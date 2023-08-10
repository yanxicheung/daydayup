#include "libwebsockets.h"
#include <signal.h>
#include <string.h>

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
    bool bin;
    bool fin;
};

// user ������������;
static int protocol_my_callback( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    struct session_data *data = (struct session_data *) user;
    switch ( reason ) {
        case LWS_CALLBACK_ESTABLISHED:       // ���������Ϳͻ���������ֺ�
            printf("Client Connect!\n");
            break;
        case LWS_CALLBACK_RECEIVE:           // �����յ��ͻ��˷�����֡�Ժ�
        {
            data->fin = lws_is_final_fragment(wsi);   // �ж��Ƿ����һ֡
            data->bin = lws_frame_is_binary(wsi);     // �ж��Ƿ��������Ϣ

            // �Է������Ľ��ն˽����������ƣ�����������������Կ���֮
            // ����ĵ��ý�ֹ�ڴ������Ͻ�������
       	    lws_rx_flow_control(wsi, 0);

            // ҵ�����֣�Ϊ��ʵ��Echo���������ѿͻ������ݱ�������
            memcpy(&data->buf[ LWS_PRE ], in, len);
            data->len = len;
            char buff[100] = {0};
            memcpy(buff, in, len);
            printf("fin[%d], is binary[%d]\n", data->fin, data->bin);
            printf("recvied msg size[%ld] content[%s]\n", len, buff);

            lws_callback_on_writable(wsi);   // ��Ҫ���ͻ���Ӧ��ʱ������һ��д�ص� LWS_CALLBACK_SERVER_WRITEABLE
        }
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:   // �������ӿ�дʱ
        	 printf("LWS_CALLBACK_SERVER_WRITEABLE======>\n");
             lws_write( wsi, &data->buf[ LWS_PRE ], data->len, LWS_WRITE_TEXT );

            lws_rx_flow_control( wsi, 1);  // ����ĵ��������ڴ������Ͻ�������
            break;
        case LWS_CALLBACK_CLOSED:
        	 printf("LWS_CALLBACK_CLOSED======>\n");
        	 break;
    }
    // �ص���������Ҫ����0�������޷�����������
    return 0;
}

/**
 * ֧�ֵ�WebSocket��Э������
 * ��Э�鼴JavaScript�ͻ���WebSocket(url, protocols)��2���������Ԫ��
 * ��Ӧ����Websocket����ʱHTTP��Я����Sec-WebSocket-Protocol�ֶ�
 * ����ҪΪÿ��Э���ṩ�ص�����
 */
struct lws_protocols protocols[] = {
    {
        //Э�����ƣ�Э��ص������ջ�������С
        "daniel", protocol_my_callback, sizeof( struct session_data ), MAX_PAYLOAD_SIZE,
    },
    {
        NULL, NULL,   0 // ���һ��Ԫ�ع̶�Ϊ�˸�ʽ
    }
};

int main(int argc,char **argv)
{
    // �źŴ�����
    signal( SIGTERM, sighdl );

    struct lws_context_creation_info ctx_info = { 0 };
    ctx_info.port = 28709;
    ctx_info.iface = NULL; // ����������ӿ��ϼ���
    ctx_info.protocols = protocols;
    ctx_info.gid = -1;
    ctx_info.uid = -1;
    ctx_info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;

    struct lws_context *context = lws_create_context(&ctx_info);
    while ( !exit_sig ) {
        lws_service(context, 1000);
    }
    lws_context_destroy(context);

    return 0;
}
