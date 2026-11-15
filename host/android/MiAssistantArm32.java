package org.rafaelia.fastboot;

/** Read-only Redmi 12 Fastboot assistant facade for Android ARM32 hosts. */
public final class MiAssistantArm32 {
    public static final String TARGET_FAMILY = "redmi-12";
    public static final String EXPECTED_PRODUCT = "fire";
    private MiAssistantArm32() {}

    public static final class Report {
        public final String product, serial, slotCount;
        public final boolean targetMatched, hyperOsObserved;
        Report(String p, String s, String slots, boolean matched, boolean hyperOs) {
            product=p; serial=s; slotCount=slots;
            targetMatched=matched; hyperOsObserved=hyperOs;
        }
    }

    /** Only bounded getvar reads; no write, unlock, erase, flash, boot or reboot. */
    public static Report inspect(AndroidFastbootUsbTransport transport, GetvarReader reader) {
        if (transport == null || reader == null)
            throw new IllegalArgumentException("null assistant dependency");
        String product=clean(reader.get("product"));
        String serial=clean(reader.get("serialno"));
        String slots=clean(reader.get("slot-count"));
        String os=clean(reader.get("android-version"));
        return new Report(product, serial, slots, EXPECTED_PRODUCT.equals(product),
            os.contains("hyperos") || os.contains("HyperOS"));
    }

    private static String clean(String value) { return value == null ? "" : value.trim(); }

    /** Reply parsing remains owned by shared C11 L0/JNI. */
    public interface GetvarReader { String get(String key); }
}
