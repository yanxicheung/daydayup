#include "libwebsockets.h"
#include <signal.h>

static volatile int exit_sig = 0;
#define MAX_PAYLOAD_SIZE  10 * 1024

void sighdl( int sig ) {
    lwsl_notice( "%d traped", sig );
    exit_sig = 1;
}

/**
 * �Ự�����Ķ��󣬽ṹ������Ҫ�Զ���
 */
struct session_data {
    int msg_count;
    unsigned char buf[LWS_PRE + MAX_PAYLOAD_SIZE];
    int len;
};

/**
 * ĳ��Э���µ����ӷ����¼�ʱ��ִ�еĻص�����
 *
 * wsi��ָ��WebSocketʵ����ָ��
 * reason�����»ص����¼�
 * user ��Ϊÿ��WebSocket�Ự������ڴ�ռ� ����������
 * in ĳЩ�¼�ʹ�ô˲�������Ϊ�������ݵ�ָ��
 * len ĳЩ�¼�ʹ�ô˲�����˵���������ݵĳ���
 */
int callback( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len ) {
    struct session_data *data = (struct session_data *) user;
    switch ( reason ) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:   // ���ӵ���������Ļص�
        {
            lwsl_notice( "Connected to server ok!\n" );
        }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:       // ���յ����������ݺ�Ļص�������Ϊin���䳤��Ϊlen
            lwsl_notice( "Rx: %s\n", (char *) in );
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE:     // ���˿ͻ��˿��Է�������ʱ�Ļص�
            if ( data->msg_count < 3 ) {
                // ǰ��LWS_PRE���ֽڱ�������LWS
                memset( data->buf, 0, sizeof( data->buf ));
                char *msg = (char *) &data->buf[ LWS_PRE ];
                data->len = sprintf( msg, "danielzpp%d", ++data->msg_count );
                lwsl_notice( "Tx====>: %s\n", msg );
                // ͨ��WebSocket�����ı���Ϣ
                lws_write( wsi, &data->buf[ LWS_PRE ], data->len, LWS_WRITE_TEXT);
            }
            break;
    }
    return 0;
}

/**
 * ֧�ֵ�WebSocket��Э������
 * ��Э�鼴JavaScript�ͻ���WebSocket(url, protocols)��2���������Ԫ��
 * ����ҪΪÿ��Э���ṩ�ص�����
 */
struct lws_protocols protocols[] = {
    {
        //Э�����ƣ�Э��ص������ջ�������С
        "daniel", callback, sizeof( struct session_data ), MAX_PAYLOAD_SIZE,
    },
    {
        NULL, NULL,   0 // ���һ��Ԫ�ع̶�Ϊ�˸�ʽ
    }
};

int main() {
    // �źŴ�����
    signal( SIGTERM, sighdl );

    // ���ڴ���vhost����context�Ĳ���
    struct lws_context_creation_info ctx_info = { 0 };
    ctx_info.port = CONTEXT_PORT_NO_LISTEN;
    ctx_info.iface = NULL;
    ctx_info.protocols = protocols;
    ctx_info.gid = -1;
    ctx_info.uid = -1;

    // ����һ��WebSocket������
    struct lws_context *context = lws_create_context(&ctx_info);

    char address[] = "10.67.76.9";
    int port = 28709;
    char addr_port[256] = { 0 };
    sprintf(addr_port, "%s:%u", address, port & 65535 );

    // �ͻ������Ӳ���
    struct lws_client_connect_info conn_info = { 0 };
    conn_info.context = context;
    conn_info.address = address;
    conn_info.port = port;
    conn_info.path = 0;
    conn_info.host = addr_port;
    conn_info.origin = addr_port;
    conn_info.protocol = protocols[ 0 ].name;

    // ����ĵ��ô���LWS_CALLBACK_PROTOCOL_INIT�¼�
    // ����һ���ͻ�������
    struct lws *wsi = lws_client_connect_via_info( &conn_info );
    while ( !exit_sig ) {
        // ִ��һ���¼�ѭ����Poll������ȴ�1000����
        lws_service( context, 1000 );
        /**
         * ����ĵ��õ������ǣ������ӿ��Խ���������ʱ������һ��WRITEABLE�¼��ص�
         * ���������ں�̨��������ʱ�������ܽ����µ�����д����������WRITEABLE�¼��ص�����ִ��
         */
        lws_callback_on_writable(wsi);
    }
    // ���������Ķ���
    lws_context_destroy( context );

    return 0;
}
