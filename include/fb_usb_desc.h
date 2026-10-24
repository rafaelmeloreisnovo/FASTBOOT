#ifndef FB_USB_DESC_H
#define FB_USB_DESC_H
/* Freestanding USB configuration-descriptor scanner. No OS, runtime, heap. */
typedef struct {
  unsigned int interface_number;
  unsigned int ep_out;
  unsigned int ep_in;
} fb_usb_binding;
/* Returns 1 only for an unambiguous FF/42/03 interface with two BULK EPs.
 * On any failure, zeroes all output fields (if output pointer is valid).
 * In/out endpoint parameters contain full bEndpointAddress including direction.
 */
unsigned int fb_usb_match_config(const unsigned char *raw, unsigned int n,
 unsigned int iface, unsigned int epout, unsigned int epin,
 fb_usb_binding *result);
#endif
