/* Pro-Type Kernel v1.3		*/
/* IPC v0.1			*/
/* By LegendMythe		*/


#include <scheduler.h>
#include <heap.h>
#include <mutex.h>
#include <apic.h>
#include <acpi.h>

class ipc_message_c
{
private:
	message_t *message;
public:
	ipc_message_c(void *buf, uint64_t sz);
	ipc_send_message(uint32_t thid);
}

struct message_c
{
	message_c *messages;

	int send_message
}

int ipc_send_message(void *buffer, uint64_t sz, uint32_t thid)
{
	thread_t *dest = tm_thread_get_by_thid(thid);

	if(dest->inbox)
	{
		iterator = dest->inbox;
		while(iterator->next)
			iterator = iterator->next;
	}
	else
	{
		dest->inbox = message;
	}
}
