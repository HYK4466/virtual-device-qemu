#include "qemu/osdep.h"
#include "hw/virtio/virtio.h"
#include "qemu/iov.h"
#include "qemu/module.h"
#include "qapi/error.h"

#include "hw/virtio/virtio-custom.h"

#define VIRTIO_ID_CUSTOM 42



static void custom_handle_output(VirtIODevice* vdev, VirtQueue* vq) {
    VirtIOCustom* s = VIRTIO_CUSTOM(vdev);
    VirtQueueElement* el;

    while ( (el = virtqueue_pop(vq, sizeof(VirtQueueElement))) != NULL) {
        size_t out_len = iov_size(el->out_sg, el->out_num);
        size_t in_len = iov_size(el->in_sg, el->in_num);
        size_t n = MIN(out_len, in_len);

        if (n > 0) {
            uint8_t *tmp = g_malloc(n);

            iov_to_buf(el->out_sg, el->out_num, 0, tmp, n);
            iov_from_buf(el->in_sg, el->in_num, 0, tmp, n);
            
            g_free(tmp);
        }

        virtqueue_push(vq, el, (unsigned)n);
        g_free(el);
    }

    virtio_notify(vdev, vq);
}

static uint64_t custom_get_features(VirtIODevice* vdev, uint64_t f, Error** errp) {
    f |= (1ULL << VIRTIO_F_VERSION_1);

    return f;
}

static void custom_set_features(VirtIODevice* vdev, uint64_t features) {
    VirtIOCustom* vc = VIRTIO_CUSTOM(vdev);
    vc->features = features;
}

static void custom_reset(VirtIODevice* vdev) {
    VirtIOCustom* vc = VIRTIO_CUSTOM(vdev);
    vc->features = 0;
}

static void custom_realize(DeviceState* qdev, Error** errp) {
    VirtIODevice* vdev = VIRTIO_DEVICE(qdev);
    VirtIOCustom* vc = VIRTIO_CUSTOM(qdev);

    virtio_init(vdev, VIRTIO_ID_CUSTOM, 0);

    vc->vq_rx = virtio_add_queue(vdev, 256, custom_handle_output);
}

static void custom_unrealize(DeviceState *qdev) {
    VirtIODevice* vdev = VIRTIO_DEVICE(qdev);
    virtio_cleanup(vdev);
}

static void custom_class_init(ObjectClass* class, const void* data) {
    DeviceClass *dc = DEVICE_CLASS(class);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(class);

    dc->realize = custom_realize;
    dc->unrealize = custom_unrealize;

    vdc->get_features = custom_get_features;
    vdc->set_features = custom_set_features;
    vdc->reset = custom_reset;

    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static const TypeInfo custom_info = {
    .name   = TYPE_VIRTIO_CUSTOM,
    .parent = TYPE_VIRTIO_DEVICE,
    .instance_size = sizeof(VirtIOCustom),
    .class_init = custom_class_init,
};

static void custom_register_types(void) {
    type_register_static(&custom_info);
}

type_init(custom_register_types);