package org.rafaelia.fastboot;

/**
 * Pure read-only Redmi 12 identity classifier.
 *
 * USB I/O remains owned by AndroidFastbootUsbTransport. This class consumes
 * already parsed getvar values and never promotes Android version to proof of
 * a HyperOS build.
 */
public final class MiAssistantArm32 {
    public static final String TARGET_FAMILY = "redmi-12";
    public static final String TARGET_MODEL = "23053RN02A";
    public static final String EXPECTED_PRODUCT = "fire";
    public static final String OBSERVED_ANDROID = "15";
    public static final String OBSERVED_HYPEROS_BUILD = "2.0.209.0.VMXMIXM";
    public static final String HYPEROS_EVIDENCE =
        "TOKEN_VAZIO_FASTBOOT_ANDROID_VERSION_DOES_NOT_PROVE_HYPEROS_BUILD";
    public static final String HOST_ABI_ARM32 = "armeabi-v7a";
    public static final String HOST_ABI_ARM64 = "arm64-v8a";

    private MiAssistantArm32() {}

    public static final class Report {
        public final String product;
        public final String serial;
        public final String slotCount;
        public final String androidVersion;
        public final boolean targetMatched;
        public final String hyperOsEvidence;

        Report(String product, String serial, String slotCount,
               String androidVersion, boolean targetMatched) {
            this.product = product;
            this.serial = serial;
            this.slotCount = slotCount;
            this.androidVersion = androidVersion;
            this.targetMatched = targetMatched;
            this.hyperOsEvidence = HYPEROS_EVIDENCE;
        }
    }

    /** Classifies bounded, parsed getvar replies; performs no USB operation. */
    public static Report inspect(GetvarReader reader) {
        if (reader == null)
            throw new IllegalArgumentException("null getvar reader");
        String product = clean(reader.get("product"));
        String serial = clean(reader.get("serialno"));
        String slots = clean(reader.get("slot-count"));
        String android = clean(reader.get("android-version"));
        return new Report(product, serial, slots, android,
                          EXPECTED_PRODUCT.equals(product));
    }

    private static String clean(String value) {
        return value == null ? "" : value.trim();
    }

    public interface GetvarReader {
        String get(String key);
    }
}
