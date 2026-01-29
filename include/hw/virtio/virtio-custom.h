#ifndef HW_VIRTIO_CUSTOM_H
#define HW_VIRTIO_CUSTOM_H

#include "qemu/osdep.h"
#include "hw/virtio/virtio.h"
#include "qemu/iov.h"
#include "qemu/module.h"
#include "qapi/error.h"

#define TYPE_VIRTIO_CUSTOM "virtio-custom"
OBJECT_DECLARE_SIMPLE_TYPE(VirtIOCustom, VIRTIO_CUSTOM)


struct VirtIOCustom {
    VirtIODevice parent_obj;
    VirtQueue* vq_rx;

    uint64_t features;
};

#endif
