OSP提供C/C++接口，内部设计采用C++；

COsp是一个全局变量；OspInit操作的就是这个全局变量；



```cpp
class COsp
{
public:
    CAppPool m_cAppPool; // application pool
    CDispatchPool m_cDispatchPool; // dispatch task pool
	CNodePool m_cNodePool;   // node Pool
	//COspLog m_cOspLog; // osplog Task
	CTaskList m_cTaskList; // service tasks list

    BOOL32 m_bBlock; // whether the system is a blocking system
	BOOL32 m_bKillOsp; // whether user called OspQuit
	BOOL32 m_bInitd; // whether user called OspInit

    COspAppDesc m_cOspAppDesc; // application's descriptiong
    COspEventDesc m_cOspEventDesc; // event description pool
	BOOL32 m_bStatusPrtEnable;  // enable status message output every times
	BOOL32 m_bCmdFuncEnable; // enable Osp functions be called on command line

	SEMHANDLE m_tSyncSema; // semaphore used to sync communication
	SEMHANDLE m_tMutexSema; // semaphore used to protect global sync ack
	BOOL32 m_bSyncAckExpired; // flag for global sync timeout
	u16 m_wSyncAckLen; // global syncack length
	u8 m_achSyncAck[MAX_MSG_LEN]; // global syncack content
    char m_achShellPrompt[MAX_PROMPT_LEN]; // shell prompt

	/*定长内存区*/
	COspStack* m_pcInstTimeStack;  //实例定时器模块内存区
	COspMemPool m_cOspInerMemPool;
}
```



OSP初始化时，进行如下操作：

1. 初始化节点（CNodeItem，节点用于OSP各个节点间的通信  **重要**）；
2. 初始化内存池（COspStack）；
3. 日志（COspLog，全局变量）；
4. 结点间消息发送任务创建（DispatchSysInit）；
5. ？？？ 创建互连的两个内部套接字。
6. 定时器模块
7. telnet模块（创建了一个OspTeleDaemon线程）
8. PostDaemon线程（守护线程, 等待客户端的接入, 等待其他节点的消息输入并转发）
9. 初始化事件描述；
10. 注册调试命令；



# App创建：

typedef zTemplate< CAppInstMsg, 1, CAppInstMsgData, 1> CAppMsg;

CreateApp

OspSend

OspPost



创建邮箱OspCreateMailbox  

```cpp
typedef struct
{
    mqd_t m_nMQHandle;
    s8 m_achMQPath[MAX_MSG_QUEUE_PATH_LENGTH];
 }TOspMQInfo;

mq_open
TOspMQInfo g_atOspMQInfo[MAX_MSG_QUEUE_PER_PROCESS] = {0};
```















# telnet：





# log：





