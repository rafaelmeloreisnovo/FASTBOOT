#include "fb_usb_desc.h"
_Static_assert(sizeof(unsigned int) == 4, "USB descriptor ABI requires 32-bit unsigned int");
unsigned int fb_usb_match_config(const unsigned char *raw, unsigned int n,
 unsigned int iface, unsigned int epout, unsigned int epin,
 fb_usb_binding *result) {
  unsigned int pos, selected, active, ep_seen, out_seen, in_seen;
  if (!result) return 0u;
  result->interface_number=0u;result->ep_out=0u;result->ep_in=0u;
  if (!raw || n < 9u || n > 1024u || iface > 255u || epout < 1u || epout > 15u ||
      epin < 129u || epin > 143u || raw[0] != 9u || raw[1] != 2u ||
      raw[2] + ((unsigned int)raw[3] << 8u) != n || raw[4] == 0u) return 0u;
  pos=9u;selected=0u;active=0u;ep_seen=0u;out_seen=0u;in_seen=0u;
  while(pos<n) {
    unsigned int step, kind;
    if(n-pos<2u) return 0u;
    step=raw[pos];kind=raw[pos+1u];
    if(step<2u || step>n-pos) return 0u;
    if(kind==4u) {
      if(step<9u) return 0u;
      if(active) {
        if(ep_seen!=2u || !out_seen || !in_seen) return 0u;
        selected++;
      }
      active=0u;ep_seen=0u;out_seen=0u;in_seen=0u;
      if(raw[pos+2u]==iface && raw[pos+3u]==0u) {
        if(raw[pos+4u]!=2u || raw[pos+5u]!=255u ||
           raw[pos+6u]!=66u || raw[pos+7u]!=3u || selected!=0u) return 0u;
        active=1u;
      }
    } else if(kind==5u && active) {
      unsigned int addr, maxpacket;
      if(step<7u || ep_seen>=2u) return 0u;
      addr=raw[pos+2u];
      maxpacket=raw[pos+4u]+((unsigned int)raw[pos+5u]<<8u);
      if((raw[pos+3u]&3u)!=2u || maxpacket==0u) return 0u;
      if(addr==epout && !out_seen) out_seen=1u;
      else if(addr==epin && !in_seen) in_seen=1u;
      else return 0u;
      ep_seen++;
    } else if(kind==2u) {
      /* Nested configuration is not permitted in the selected configuration. */
      return 0u;
    }
    pos+=step;
  }
  if(active) {
    if(ep_seen!=2u || !out_seen || !in_seen) return 0u;
    selected++;
  }
  if(selected!=1u) return 0u;
  result->interface_number=iface;result->ep_out=epout;result->ep_in=epin;
  return 1u;
}
