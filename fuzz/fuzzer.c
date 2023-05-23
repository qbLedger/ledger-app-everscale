#ifdef FUZZ_WITH_PROTOBUF
#include <bits/types/struct_timeval.h>
#include <src/libfuzzer/libfuzzer_macro.h>
#include <sys/time.h>

#include "proto/pcap.pb.h"
#endif

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "globals.h"
#include "message.h"
#include "os.h"
#include "byte_stream.h"

#ifdef FUZZ_WITH_PROTOBUF
#else

BocContext_t boc_context;
DataContext_t data_context;

extern int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len) {
    // if (len < 40) return -1;

    BEGIN_TRY {
        TRY {
            ByteStream_t src;
            ByteStream_init(&src, buf, len);

            uint8_t wc = DEFAULT_WORKCHAIN_ID;

            uint8_t address[ADDRESS_LENGTH] = {
                0x30, 0x99, 0xf1, 0x4e, 0xcc, 0xaa, 0x05, 0x42,
                0xd2, 0xd6, 0x0e, 0x92, 0xeb, 0x66, 0x49, 0x5f,
                0x6e, 0xcf, 0x01, 0xa1, 0x14, 0xe1, 0x2f, 0x9d,
                0xb8, 0xd9, 0xcb, 0x82, 0x7a, 0x87, 0xbf, 0x84};

            uint8_t prepend_address[ADDRESS_LENGTH];
            memcpy(prepend_address, address, ADDRESS_LENGTH);

            int flow = prepare_to_sign(&src, wc, address, prepend_address);
        }
        CATCH_OTHER(e) {}
        FINALLY {}
    }
    END_TRY;

    return 0;
}

#endif

#ifdef IS_AFL

__AFL_FUZZ_INIT();

int main(int argc, char *argv[]) {
#ifdef __AFL_HAVE_MANUAL_CONTROL
    __AFL_INIT();
#endif

    uint8_t *buf = __AFL_FUZZ_TESTCASE_BUF;

    while (__AFL_LOOP(10000)) {
        int len = __AFL_FUZZ_TESTCASE_LEN;
        LLVMFuzzerTestOneInput(buf, len);
    }

    return 0;
}

#endif