package org.rafaelia.fastboot;

/** Read-only Redmi 12 Fastboot facade on Android ARM32 or ARM64 host. */
public final class MiAssistantArm32 {
    public static final String TARGET_FAMILY = "redmi-12";
    public static final String TARGET_MODEL = "23053RN02A";
    public static final String EXPECTED_PRODUCT = "fire";
    public static final String TARGET_ANDROID = "15";
    public static final String TARGET_HYPEROS_BUILD = "2.0.209.0.VMXMIXM";
    public static final String HOST_ABI_ARM32 = "armeabi-v7a";
    public static final String HOST_ABI_ARM64 = "arm64-v8a";
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
            os.toLowerCase(java.util.Locale.ROOT).contains("hyperos"));
    }

    private static String clean(String value) { return value == null ? "" : value.trim(); }
    public interface GetvarReader { String get(String key); }
}
