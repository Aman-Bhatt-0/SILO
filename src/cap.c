#include "cap.h"

int drop_capabilities() {
    fprintf(stderr, "=> setting up process capabilities...");

    int caps_list[] = {
        CAP_SYS_ADMIN
    };

    size_t num_caps = sizeof(caps_list) / sizeof(*caps_list);

    fprintf(stderr, "bounding...");
    for (size_t i = 0; i < num_caps; i++) {
        if (prctl(PR_CAPBSET_DROP, caps_list[i], 0, 0, 0)) {
            fprintf(stderr, "prctl failed: %m\n");
            return 1;
        }
    }

    fprintf(stderr, "inheritable...");
    cap_t caps = NULL;

    if (!(caps = cap_get_proc())
        || cap_set_flag(caps, CAP_INHERITABLE, num_caps, caps_list, CAP_CLEAR)
        || cap_set_proc(caps)) {
        fprintf(stderr, "failed: %m\n");
        if (caps) cap_free(caps);
        return 1;
    }

    cap_free(caps);
    fprintf(stderr, "done.\n");

    return 0;
}

