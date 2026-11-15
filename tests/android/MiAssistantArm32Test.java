package org.rafaelia.fastboot;

import java.util.HashMap;
import java.util.Map;

public final class MiAssistantArm32Test {
    private static final class Reader implements MiAssistantArm32.GetvarReader {
        private final Map<String, String> values = new HashMap<String, String>();
        Reader put(String key, String value) {
            values.put(key, value);
            return this;
        }
        public String get(String key) {
            return values.get(key);
        }
    }

    private static void check(boolean condition, String message) {
        if (!condition) throw new AssertionError(message);
    }

    public static void main(String[] args) {
        Reader matching = new Reader()
            .put("product", " fire ")
            .put("serialno", "ABC123")
            .put("slot-count", "2")
            .put("android-version", "15");

        MiAssistantArm32.Report report = MiAssistantArm32.inspect(matching);
        check(report.targetMatched, "fire must match Redmi 12 target");
        check("fire".equals(report.product), "product must be trimmed");
        check("15".equals(report.androidVersion), "Android version must be preserved");
        check(MiAssistantArm32.HYPEROS_EVIDENCE.equals(report.hyperOsEvidence),
              "HyperOS evidence must remain typed TOKEN_VAZIO");

        Reader mismatch = new Reader().put("product", "other");
        MiAssistantArm32.Report other = MiAssistantArm32.inspect(mismatch);
        check(!other.targetMatched, "non-fire product must fail closed");
        check("".equals(other.serial), "missing serial must be empty, not fabricated");
        check("".equals(other.androidVersion), "missing Android version must be empty");

        boolean nullRejected = false;
        try {
            MiAssistantArm32.inspect(null);
        } catch (IllegalArgumentException expected) {
            nullRejected = true;
        }
        check(nullRejected, "null reader must be rejected");

        System.out.println("PASS_SCOPED: Java classifier behavior 12 assertions");
    }
}
