#include <stdio.h>
#include <string>

//#include "ble.h"
#include "sd_rpc.h"

#define PORT_NAME_SIZE  (12)

static void show_usage(void);
static const char * get_test_settings(
    int const * const argc,
    char const * const argv[]
);

int main(int argc, char* argv[]) {
    auto port = get_test_settings(&argc, argv);
    if (port) {
#ifdef _DEBUG
        printf("Port: %s\n", port);
#endif // _DEBUG
        uint32_t err_lvl = 0;
        adapter_t * m_adapter = nullptr;
        {
            physical_layer_t  * phy = sd_rpc_physical_layer_create_uart(
                port,
                1000000,
                SD_RPC_FLOW_CONTROL_NONE,
                SD_RPC_PARITY_NONE
            );
            auto datalink = sd_rpc_data_link_layer_create_bt_three_wire(phy, 250);
            auto transport = sd_rpc_transport_layer_create(datalink, 1500);

            m_adapter = sd_rpc_adapter_create(transport);
        }

        sd_rpc_log_handler_severity_filter_set(m_adapter, SD_RPC_LOG_INFO);
//        err_lvl = sd_rpc_open(m_adapter, status_handler, ble_evt_dispatch, log_handler);
    }
    else {
        printf("Port is not specified\n");
    }

    printf("Press Enter to finish.\n");
    fflush(stdout);
    getchar();
    return 0;
}

static const char * get_test_settings(
    int const * const argc,
    char const * const argv[]
) {
    const char * ret_value = nullptr;
    static char port[PORT_NAME_SIZE] = {};
    if (*argc > 1 && std::string(argv[1]) == "-h") {
        show_usage();
        return ret_value;
    }
    uint8_t param_count = 1;
    for (int i = 1; i < *argc;) {
        if (std::string(argv[i]) == "-p" && *argc > i + 1) {
            char const * const port_name = argv[i + 1];
            uint8_t count = 0;
            while (count < PORT_NAME_SIZE) {
                port[count] = *(port_name + count);
                if (!*(port_name + count)) break;
                count++;
            }
            i += 2;
            param_count++;
            ret_value = port;
            continue;
        }
        
        printf(
            "\nError: the parameter #%i (%s) is not a valid parameter in this "
            "application.\nThe execution has been stopped.\n\n",
            param_count, argv[i]
        );

        show_usage();
        return nullptr;
    }
    return ret_value;
}

static void show_usage(void) {
    printf(
        "The application can be used to run a set of tests.\n\n"
        "usage: test [-p <port name>]\n  or : test -h\n\n"
        "\t-p <name> - specifies serial port name.\n"
        "\t-h        - printing of usage help.\n\n"
        "Usage examples:\n  test -p COM3\n  test -h\n\n"
    );
}
/*---------------------------------------------------------------------------*/