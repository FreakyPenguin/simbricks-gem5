#ifndef COSIM_PCIE_PROTO_H_
#define COSIM_PCIE_PROTO_H_

#include <stdint.h>

/******************************************************************************/
/* Initialization messages on Unix socket */

/** in dev_intro.flags to indicate that sender supports issuing syncs. */
#define COSIM_PCIE_PROTO_FLAGS_DI_SYNC (1 << 0)

/** Number of PCI bars */
#define COSIM_PCIE_PROTO_NBARS 6

/** in bars.flags: this is an I/O port bar. (otherwise memory) */
#define COSIM_PCIE_PROTO_BAR_IO (1 << 0)
/** in bars.flags: this is a 64-bit bar. (otherwise 32-bit only) */
#define COSIM_PCIE_PROTO_BAR_64 (1 << 1)
/** in bars.flags: this memory bar is prefetchable */
#define COSIM_PCIE_PROTO_BAR_PF (1 << 2)

/**
 * welcome message sent by device to host. This message comes with the shared
 * memory file descriptor attached.
 */
struct cosim_pcie_proto_dev_intro {
    /** flags: see COSIM_PCIE_PROTO_FLAGS_DI_* */
    uint64_t flags;

    /** offset of the device-to-host queue in shared memory region */
    uint64_t d2h_offset;
    /** size of an entry in the device-to-host queue in bytes */
    uint64_t d2h_elen;
    /** total device-to-host queue length in #entries */
    uint64_t d2h_nentries;

    /** offset of the host-to-device queue in shared memory region */
    uint64_t h2d_offset;
    /** size of an entry in the host-to-device queue in bytes */
    uint64_t h2d_elen;
    /** total host-to-device queue length in #entries */
    uint64_t h2d_nentries;

    /** information for each BAR exposed by the device */
    struct {
        /** length of the bar in bytes (len = 0 indicates unused bar) */
        uint64_t len;
        /** flags (see COSIM_PCIE_PROTO_BAR_*) */
        uint64_t flags;
    } __attribute__((packed)) bars[COSIM_PCIE_PROTO_NBARS];

    /** PCI vendor id */
    uint16_t pci_vendor_id;
    /** PCI device id */
    uint16_t pci_device_id;
    /* PCI class */
    uint8_t pci_class;
    /* PCI subclass */
    uint8_t pci_subclass;
    /* PCI revision */
    uint8_t pci_revision;
} __attribute__((packed));


#define COSIM_PCIE_PROTO_FLAGS_HI_SYNC (1 << 0)

/** welcome message sent by host to device */
struct cosim_pcie_proto_host_intro {
    /** flags: see COSIM_PCIE_PROTO_FLAGS_HI_* */
  uint64_t flags;
} __attribute__((packed));


/******************************************************************************/
/* Messages on in-memory device to host channel */

/** Mask for ownership bit in own_type field */
#define COSIM_PCIE_PROTO_D2H_OWN_MASK 0x80
/** Message is owned by device */
#define COSIM_PCIE_PROTO_D2H_OWN_DEV 0x00
/** Message is owned by host */
#define COSIM_PCIE_PROTO_D2H_OWN_HOST 0x80

/** Mask for type value in own_type field */
#define COSIM_PCIE_PROTO_D2H_MSG_MASK 0x7f
#define COSIM_PCIE_PROTO_D2H_MSG_SYNC 0x1
#define COSIM_PCIE_PROTO_D2H_MSG_READ 0x2
#define COSIM_PCIE_PROTO_D2H_MSG_WRITE 0x3
#define COSIM_PCIE_PROTO_D2H_MSG_INTERRUPT 0x4
#define COSIM_PCIE_PROTO_D2H_MSG_READCOMP 0x5
#define COSIM_PCIE_PROTO_D2H_MSG_WRITECOMP 0x6

struct cosim_pcie_proto_d2h_dummy {
    uint8_t pad[63];
    uint8_t own_type;
} __attribute__((packed));

struct cosim_pcie_proto_d2h_sync {
    uint64_t timestamp;
    uint8_t pad[55];
    uint8_t own_type;
} __attribute__((packed));


struct cosim_pcie_proto_d2h_read {
    uint64_t req_id;
    uint64_t offset;
    uint16_t len;
    uint8_t pad[45];
    uint8_t own_type;
} __attribute__((packed));

struct cosim_pcie_proto_d2h_write {
    uint64_t req_id;
    uint64_t offset;
    uint16_t len;
    uint8_t pad[45];
    uint8_t own_type;
    uint8_t data[];
} __attribute__((packed));


#define COSIM_PCIE_PROTO_INT_LEGACY_HI 0
#define COSIM_PCIE_PROTO_INT_LEGACY_LO 1
#define COSIM_PCIE_PROTO_INT_MSI 2
#define COSIM_PCIE_PROTO_INT_MSIX 3

struct cosim_pcie_proto_d2h_interrupt {
    uint16_t vector;
    uint8_t inttype;
    uint8_t pad[60];
    uint8_t own_type;
} __attribute__((packed));

struct cosim_pcie_proto_d2h_readcomp {
    uint64_t req_id;
    uint8_t pad[55];
    uint8_t own_type;
    uint8_t data[];
};

struct cosim_pcie_proto_d2h_writecomp {
    uint64_t req_id;
    uint8_t pad[55];
    uint8_t own_type;
};

union cosim_pcie_proto_d2h {
    struct cosim_pcie_proto_d2h_dummy dummy;
    struct cosim_pcie_proto_d2h_sync sync;
    struct cosim_pcie_proto_d2h_read read;
    struct cosim_pcie_proto_d2h_write write;
    struct cosim_pcie_proto_d2h_interrupt interrupt;
    struct cosim_pcie_proto_d2h_readcomp readcomp;
    struct cosim_pcie_proto_d2h_writecomp writecomp;
};


/******************************************************************************/
/* Messages on in-memory host to device channel */

#define COSIM_PCIE_PROTO_H2D_OWN_MASK 0x80
/** Message is owned by host */
#define COSIM_PCIE_PROTO_H2D_OWN_HOST 0x00
/** Message is owned by device */
#define COSIM_PCIE_PROTO_H2D_OWN_DEV 0x80

#define COSIM_PCIE_PROTO_H2D_MSG_MASK 0x7f
#define COSIM_PCIE_PROTO_H2D_MSG_SYNC 0x1
#define COSIM_PCIE_PROTO_H2D_MSG_READ 0x2
#define COSIM_PCIE_PROTO_H2D_MSG_WRITE 0x3
#define COSIM_PCIE_PROTO_H2D_MSG_READCOMP 0x4
#define COSIM_PCIE_PROTO_H2D_MSG_WRITECOMP 0x5

struct cosim_pcie_proto_h2d_dummy {
    uint8_t pad[63];
    uint8_t own_type;
} __attribute__((packed));

struct cosim_pcie_proto_h2d_sync {
    uint64_t timestamp;
    uint8_t pad[55];
    uint8_t own_type;
} __attribute__((packed));

struct cosim_pcie_proto_h2d_read {
    uint64_t req_id;
    uint64_t offset;
    uint16_t len;
    uint8_t bar;
    uint8_t pad[44];
    uint8_t own_type;
} __attribute__((packed));

struct cosim_pcie_proto_h2d_write {
    uint64_t req_id;
    uint64_t offset;
    uint16_t len;
    uint8_t bar;
    uint8_t pad[44];
    uint8_t own_type;
    uint8_t data[];
} __attribute__((packed));

struct cosim_pcie_proto_h2d_readcomp {
    uint64_t req_id;
    uint8_t pad[55];
    uint8_t own_type;
    uint8_t data[];
};

struct cosim_pcie_proto_h2d_writecomp {
    uint64_t req_id;
    uint8_t pad[55];
    uint8_t own_type;
};

union cosim_pcie_proto_h2d {
    struct cosim_pcie_proto_h2d_dummy dummy;
    struct cosim_pcie_proto_h2d_sync sync;
    struct cosim_pcie_proto_h2d_read read;
    struct cosim_pcie_proto_h2d_write write;
    struct cosim_pcie_proto_h2d_readcomp readcomp;
    struct cosim_pcie_proto_h2d_writecomp writecomp;
};

#endif /* ndef COSIM_PCIE_PROTO_H_ */
