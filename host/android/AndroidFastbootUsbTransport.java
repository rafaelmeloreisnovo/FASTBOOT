package org.rafaelia.fastboot;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;

/** L1 Android-only transport. The C11 L0 owns protocol parsing and session state.
 *  No flash/unlock support. No implicit USB permissions or kernel bypass.
 */
public final class AndroidFastbootUsbTransport implements AutoCloseable {
    private final UsbDeviceConnection connection;
    private final UsbInterface chosen;
    private final UsbEndpoint endpointOut;
    private final UsbEndpoint endpointIn;
    private boolean closed;

    private AndroidFastbootUsbTransport(UsbDeviceConnection c, UsbInterface i,
                                       UsbEndpoint out, UsbEndpoint in) {
        connection = c; chosen = i; endpointOut = out; endpointIn = in;
    }

    /** Returns null for non-Fastboot devices; throws SecurityException if user permission is absent. */
    public static AndroidFastbootUsbTransport open(UsbManager manager, UsbDevice device) {
        if (manager == null || device == null) throw new IllegalArgumentException("null USB source");
        if (!manager.hasPermission(device)) throw new SecurityException("USB permission missing");
        for (int index = 0; index < device.getInterfaceCount(); index++) {
            UsbInterface intf = device.getInterface(index);
            if (intf.getInterfaceClass() != 0xff ||
                intf.getInterfaceSubclass() != 0x42 ||
                intf.getInterfaceProtocol() != 0x03) continue;
            UsbEndpoint in = null, out = null;
            for (int e = 0; e < intf.getEndpointCount(); e++) {
                UsbEndpoint endpoint = intf.getEndpoint(e);
                if (endpoint.getType() != UsbConstants.USB_ENDPOINT_XFER_BULK) continue;
                if (endpoint.getDirection() == UsbConstants.USB_DIR_IN) in = endpoint;
                else if (endpoint.getDirection() == UsbConstants.USB_DIR_OUT) out = endpoint;
            }
            if (in == null || out == null) continue;
            UsbDeviceConnection c = manager.openDevice(device);
            if (c == null) return null;
            if (!c.claimInterface(intf, false)) { c.close(); return null; }
            return new AndroidFastbootUsbTransport(c, intf, out, in);
        }
        return null;
    }

    /** Transmits exactly one bounded getvar packet (produced by C11 L0). */
    public int writeGetvar(byte[] bytes, int length) {
        if (closed) throw new IllegalStateException("USB transport closed");
        if (bytes == null || length < 8 || length > 64 || length > bytes.length)
            throw new IllegalArgumentException("invalid getvar packet length");
        final byte[] prefix = {'g','e','t','v','a','r',':'};
        for (int i = 0; i < prefix.length; i++)
            if (bytes[i] != prefix[i]) throw new IllegalArgumentException("only getvar allowed");
        for (int i = prefix.length; i < length; i++) {
            int c = bytes[i] & 0xff;
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                  (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.'))
                throw new IllegalArgumentException("invalid getvar key");
        }
        return connection.bulkTransfer(endpointOut, bytes, 0, length, 2500);
    }

    /** Raw 4..256-byte replies must be parsed by shared C11 L0, not by Android wrapper. */
    public int readResponse(byte[] buffer) {
        if (closed) throw new IllegalStateException("USB transport closed");
        if (buffer == null || buffer.length < 256)
            throw new IllegalArgumentException("response buffer <256");
        return connection.bulkTransfer(endpointIn, buffer, 0, 256, 2500);
    }

    @Override public void close() {
        if (!closed) {
            closed = true;
            connection.releaseInterface(chosen);
            connection.close();
        }
    }
}
