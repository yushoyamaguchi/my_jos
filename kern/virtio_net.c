#include <inc/types.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/string.h>
#include <kern/virtio.h>
#include <kern/spinlock.h>

#define R(r) ((volatile uint32_t *)(VIRTIO1 + (r)))

#define NUM 32

struct virt_queue {
    uint32_t num;
    struct virtq_desc desc[16 * VIRTQ_SIZE] __attribute__ ((aligned(16)));
    struct virtq_avail avail[6 + (2 * VIRTQ_SIZE)] __attribute__ ((aligned(2)));
    struct virtq_used used[6 + (8 * VIRTQ_SIZE)] __attribute__ ((aligned(4)));

    // our own book-keeping.
    char free[2*NUM];   // is a descriptor free?
    uint16_t used_idx;    // we've looked this far in used->ring.

    // packet headers
    // one-for-one with descriptors, for convenience.
    struct virtio_net_hdr ops[NUM];
};

struct net {
    struct virt_queue rx;
    struct virt_queue tx;
    void  *send_buf[NUM];
    void  *recv_buf[NUM];
    struct spinlock vnet_lock;
} net;


void mmio_virtq_init(struct virt_queue *q, int qidx) {
    /* 
     * MMIO-specific initialization
     * check spec 4.2.3.2 Virtqueue Configuration
     */
    
    // 1. select a queue by writing to queuesel
    *R(VIRTIO_MMIO_QUEUE_SEL) = qidx;
    
    // 2. check if the queue is already in use
    if(*R(VIRTIO_MMIO_QUEUE_READY))
        panic("virtq_init: queue already in use");
    
    // 3. check if the queue is available (queue-num-max != 0)
    uint32_t max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
    if(max == 0) panic("virtq_init: queue not available");
    if(max < NUM) panic("virtq_init: queue too short");

    memset(q->desc, 0, PGSIZE);
    memset(q->avail, 0, PGSIZE);
    memset(q->used, 0, PGSIZE);

    // 5. notify the device about the queue size
    *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;
    if (qidx == 0) // RX queue
        *R(VIRTIO_MMIO_QUEUE_NUM) = 2*NUM;

    // 6. write PA of three parts of the queue to the device
    *R(VIRTIO_MMIO_QUEUE_DESC_LOW)   = (uint32_t)q->desc;
    *R(VIRTIO_MMIO_QUEUE_DESC_HIGH)  = 0;
    *R(VIRTIO_MMIO_DRIVER_DESC_LOW)  = (uint32_t)q->avail;
    *R(VIRTIO_MMIO_DRIVER_DESC_HIGH) = 0;
    *R(VIRTIO_MMIO_DEVICE_DESC_LOW)  = (uint32_t)q->used;
    *R(VIRTIO_MMIO_DEVICE_DESC_HIGH) = 0;

    // 7. tell the device that the queue is ready
    *R(VIRTIO_MMIO_QUEUE_READY) = 1;

    /* 
     * initialize book-keeping 
     */
    for(int i = 0; i < 2*NUM; i++)
        q->free[i] = 1;
    q->used_idx = 0;
}

void virtio_net_init(void *mac){
    uint32_t status = 0;

    spin_initlock(&net.vnet_lock);

    /*
     * MMIO-specific checking.
     * spec 4.2.3.1.1 Driver Requirements: Device Initialization
     */
    if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
        *R(VIRTIO_MMIO_VERSION) != 2 ||
        *R(VIRTIO_MMIO_DEVICE_ID) != 1 ||  // 1 for network device
        *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551){
        panic("could not find virtio net");
    }

}