#include "qemu/osdep.h"
#include "hw/pci/pci.h"
#include "hw/core/qdev-properties.h"
#include "hw/virtio/virtio.h"
#include "hw/virtio/virtio-pci.h"
#include "qemu/module.h"
#include "qapi/error.h"

#include "hw/virtio/virtio-custom.h"

#define TYPE_VIRTIO_CUSTOM_PCI "virtio-custom-pci"
OBJECT_DECLARE_SIMPLE_TYPE(VirtIOCustomPCI, VIRTIO_CUSTOM_PCI)

struct VirtIOCustomPCI {
    VirtIOPCIProxy parent_obj;
    VirtIOCustom vdev;
};

static const Property virtio_custom_pci_properties[] = {
    DEFINE_PROP_UINT32("vectors", VirtIOPCIProxy, nvectors,
                       DEV_NVECTORS_UNSPECIFIED),
};

static void virtio_custom_pci_realize(VirtIOPCIProxy* vpci, Error** errp) {
    VirtIOCustomPCI* dev = VIRTIO_CUSTOM_PCI(vpci);
    DeviceState* vdev = DEVICE(&dev->vdev);

    if (vpci->nvectors == DEV_NVECTORS_UNSPECIFIED) {
        vpci->nvectors = 2;
    }

    virtio_pci_force_virtio_1(vpci);

    pci_config_set_interrupt_pin(vpci->pci_dev.config, 1);

    if (!qdev_realize(vdev, BUS(&vpci->bus), errp)) {
        return;
    }
}

static void virtio_custom_pci_class_init(ObjectClass* klass, const void* data) {
    
    DeviceClass* dc = DEVICE_CLASS(klass);
    VirtioPCIClass* k = VIRTIO_PCI_CLASS(klass);
    PCIDeviceClass* pcidev_k = PCI_DEVICE_CLASS(klass);

    k->realize = virtio_custom_pci_realize;

    dc->desc = "virtio-custom pci device";
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_set_props(dc, virtio_custom_pci_properties);

    pcidev_k->class_id = PCI_CLASS_OTHERS;
    pcidev_k->vendor_id = PCI_VENDOR_ID_REDHAT_QUMRANET;
    pcidev_k->device_id = 0x1040 + 42;
    pcidev_k->subsystem_vendor_id = PCI_VENDOR_ID_REDHAT_QUMRANET;
    pcidev_k->subsystem_id = 42;
    pcidev_k->revision = 1;
}

static void virtio_custom_pci_instance_init(Object* obj) {
    VirtIOCustomPCI *dev = VIRTIO_CUSTOM_PCI(obj);
    virtio_instance_init_common(obj, &dev->vdev, sizeof(dev->vdev), TYPE_VIRTIO_CUSTOM);
}

static const VirtioPCIDeviceTypeInfo virtio_custom_pci_info = {
    .generic_name = TYPE_VIRTIO_CUSTOM_PCI,
    .instance_size = sizeof(VirtIOCustomPCI),
    .instance_init = virtio_custom_pci_instance_init,
    .class_init = virtio_custom_pci_class_init,
};

static void virtio_custom_pci_register_types(void) {
    virtio_pci_types_register(&virtio_custom_pci_info);
}

type_init(virtio_custom_pci_register_types);
