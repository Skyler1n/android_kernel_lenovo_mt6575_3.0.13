









#include "osal_typedef.h"
#include "osal.h"



















/*string operations*/
_osal_inline_ UINT32  osal_strlen(const char *str)
{
    return strlen(str);
}

_osal_inline_ INT32 osal_strcmp(const char *dst, const char *src)
{
    return strcmp(dst, src);
}

_osal_inline_ INT32 osal_strncmp(const char *dst, const char *src, UINT32 len)
{
    return strncmp(dst, src, len);
}

_osal_inline_ char * osal_strcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
}

_osal_inline_ char * osal_strncpy(char *dst, const char *src, UINT32 len)
{
    return strncpy(dst, src, len);
}


_osal_inline_ char * osal_strcat(char *dst, const char *src)
{
    return strcat(dst, src);
}

_osal_inline_ char * osal_strncat(char *dst, const char *src, UINT32 len)
{
    return strncat(dst, src, len);
}

_osal_inline_ char * osal_strchr(const char *str, UINT8 c)
{
    return strchr(str, c);
}


_osal_inline_ char * osal_strsep(char **str, const char *c)
{
    return strsep(str, c);
}


_osal_inline_ LONG osal_strtol(const char *str, char **c, UINT32 adecimal)
{
    return simple_strtol(str, c, adecimal);
}

INT32 osal_snprintf(char *buf, UINT32 len, const char*fmt, ...)
{
    INT32 iRet = 0;
    va_list  args;

    /*va_start(args, fmt);*/
    va_start(args, fmt);
    /*iRet = snprintf(buf, len, fmt, args);*/// TODO: [FixMe][GeorgeKuo] BUG?
    iRet = vsnprintf(buf, len, fmt, args);
    va_end(args);

    return iRet;
}

INT32 osal_print(const char *str, ...)
{
    va_list  args;
    char tempString[DBG_LOG_STR_SIZE];

    va_start(args, str);
    vsnprintf(tempString, DBG_LOG_STR_SIZE, str, args);
    va_end(args);

    printk("%s",tempString);

    return 0;
}


INT32 osal_dbg_print(const char *str, ...)
{
    va_list  args;
    char tempString[DBG_LOG_STR_SIZE];

    va_start(args, str);
    vsnprintf(tempString, DBG_LOG_STR_SIZE, str, args);
    va_end(args);

    printk(KERN_INFO "%s",tempString);

    return 0;
}


INT32 osal_dbg_assert(INT32 expr, const char *file, INT32 line)
{
    if (!expr){
        printk("%s (%d)\n", file, line);
        /*BUG_ON(!expr);*/
#ifdef CFG_COMMON_GPIO_DBG_PIN
//package this part
        mt_set_gpio_out(GPIO70, GPIO_OUT_ZERO);
        printk("toggle GPIO70\n");
        udelay(10);
        mt_set_gpio_out(GPIO70, GPIO_OUT_ONE);
#endif
        return 1;
    }
    return 0;


}

INT32 osal_dbg_assert_aee(const char *module, const char *detail_description){
	osal_err_print("[WMT-ASSERT]""[E][Module]:%s, [INFO]%s\n", module, detail_description);
#ifdef WMT_PLAT_ALPS
    aee_kernel_warning(
        module,
        detail_description);
#endif
    return 0;
}

INT32 osal_sprintf(char *str, const char *format, ...)
{
    INT32 iRet = 0;
    va_list  args;

    va_start(args, format);
    iRet = vsnprintf(str, DBG_LOG_STR_SIZE, format, args);
    va_end(args);

    return iRet;
}

_osal_inline_ VOID* osal_malloc(UINT32 size)
{
    return vmalloc(size);
}


_osal_inline_ VOID osal_free(const VOID *dst)
{
    vfree(dst);
}

_osal_inline_ VOID* osal_memset(VOID *buf, INT32 i, UINT32 len)
{
    return memset(buf, i, len);
}


_osal_inline_ VOID* osal_memcpy(VOID *dst, const VOID *src, UINT32 len)
{
    return memcpy(dst, src, len);
}


_osal_inline_ INT32 osal_memcmp(const VOID *buf1, const VOID *buf2, UINT32 len)
{
    return memcmp(buf1, buf2, len);
}

_osal_inline_ INT32
osal_thread_create (
    P_OSAL_THREAD pThread
    )
{
    pThread->pThread = kthread_create(pThread->pThreadFunc,
        pThread->pThreadData,
        pThread->threadName);
    if (NULL == pThread->pThread) {
        return -1;
    }
    return 0;
}
_osal_inline_ INT32
osal_thread_run (
    P_OSAL_THREAD pThread
    )
{
    if (pThread->pThread) {
        wake_up_process(pThread->pThread);
        return 0;
    }
    else {
        return -1;
    }
}

_osal_inline_ INT32
osal_thread_stop (
    P_OSAL_THREAD pThread
    )
{
    INT32 iRet;
    if ( (pThread) && (pThread->pThread) ) {
        iRet = kthread_stop(pThread->pThread);
        //pThread->pThread = NULL;
        return iRet;
    }
    return -1;
}


_osal_inline_ INT32
osal_thread_should_stop (
    P_OSAL_THREAD pThread
    )
{
    if ( (pThread) && (pThread->pThread) ) {
        return kthread_should_stop();
    }
    else {
        return 1;
    }
}


_osal_inline_ INT32
osal_thread_wait_for_event (
    P_OSAL_THREAD pThread,
    P_OSAL_EVENT pEvent,
    P_OSAL_EVENT_CHECKER pChecker
    )
{
/*    P_DEV_WMT pDevWmt;*/

    if ( (pThread) && (pThread->pThread) && (pEvent) && (pChecker)) {
/*        pDevWmt = (P_DEV_WMT)(pThread->pThreadData);*/
        return wait_event_interruptible(pEvent->waitQueue,
            (/*!RB_EMPTY(&pDevWmt->rActiveOpQ) ||*/ osal_thread_should_stop(pThread) || (*pChecker)(pThread)));
    }
    return -1;
}

_osal_inline_ INT32
osal_thread_destroy (
    P_OSAL_THREAD pThread
    )
{
    if (pThread && (pThread->pThread)) {
        kthread_stop(pThread->pThread);
        pThread->pThread = NULL;
    }
    return 0;
}


_osal_inline_ INT32
osal_signal_init (
    P_OSAL_SIGNAL pSignal
    )
{
    if (pSignal) {
        init_completion(&pSignal->comp);
        return 0;
    }
    else {
        return -1;
    }
}

_osal_inline_ INT32
osal_wait_for_signal (
    P_OSAL_SIGNAL pSignal
    )
{
    if (pSignal) {
        wait_for_completion_interruptible(&pSignal->comp);
        return 0;
    }
    else {
        return -1;
    }
}

_osal_inline_ INT32
osal_wait_for_signal_timeout (
    P_OSAL_SIGNAL pSignal
    )
{
    /* return wait_for_completion_interruptible_timeout(&pSignal->comp, msecs_to_jiffies(pSignal->timeoutValue));*/
    /* [ChangeFeature][George] gps driver may be closed by -ERESTARTSYS.
     * Avoid using *interruptible" version in order to complete our jobs, such
     * as function off gracefully.
     */
    return wait_for_completion_timeout(&pSignal->comp, msecs_to_jiffies(pSignal->timeoutValue));
}

_osal_inline_ INT32
osal_raise_signal (
    P_OSAL_SIGNAL pSignal
    )
{
    // TODO:[FixMe][GeorgeKuo]: DO sanity check here!!!
    complete(&pSignal->comp);
    return 0;
}

_osal_inline_ INT32
osal_signal_deinit (
    P_OSAL_SIGNAL pSignal
    )
{
    // TODO:[FixMe][GeorgeKuo]: DO sanity check here!!!
    pSignal->timeoutValue = 0;
    return 0;
}



INT32 osal_event_init (
    P_OSAL_EVENT pEvent
    )
{
    init_waitqueue_head(&pEvent->waitQueue);    
    
    return 0;
}

INT32 osal_wait_for_event(
    P_OSAL_EVENT pEvent, 
    INT32 (*condition)(PVOID),
    void *cond_pa
    )
{
    return  wait_event_interruptible(pEvent->waitQueue, condition(cond_pa)); 
}

INT32 osal_wait_for_event_timeout(
       P_OSAL_EVENT pEvent,
       INT32 (*condition)(PVOID),
       void *cond_pa
       )
{
    return wait_event_interruptible_timeout(pEvent->waitQueue, condition(cond_pa), msecs_to_jiffies(pEvent->timeoutValue));
}

INT32 osal_trigger_event(
    P_OSAL_EVENT pEvent
    )
{
    INT32 ret = 0;
    wake_up_interruptible(&pEvent->waitQueue);
    return ret;
}
                                                        
INT32
osal_event_deinit (
    P_OSAL_EVENT pEvent
    )
{
    return 0;
}

_osal_inline_ LONG osal_wait_for_event_bit_set(P_OSAL_EVENT pEvent, PULONG pState, UINT32 bitOffset)
{
     UINT32 ms = pEvent->timeoutValue;
    if (ms != 0)
    {
        return wait_event_interruptible_timeout(pEvent->waitQueue,  test_bit(bitOffset, pState), msecs_to_jiffies(ms));
    }
    else
    {
        return wait_event_interruptible(pEvent->waitQueue,  test_bit(bitOffset, pState));
    }

}

_osal_inline_ LONG osal_wait_for_event_bit_clr(P_OSAL_EVENT pEvent, PULONG pState, UINT32 bitOffset)
{
    UINT32 ms = pEvent->timeoutValue;
    if (ms != 0)
    {
        return wait_event_interruptible_timeout(pEvent->waitQueue,  !test_bit(bitOffset, pState), msecs_to_jiffies(ms));
    }
    else
    {
        return wait_event_interruptible(pEvent->waitQueue,  !test_bit(bitOffset, pState));
    }

}

#if    OS_BIT_OPS_SUPPORT
#define osal_bit_op_lock(x) 
#define osal_bit_op_unlock(x) 
#else

_osal_inline_ INT32 osal_bit_op_lock(P_OSAL_UNSLEEPABLE_LOCK pLock)
{
    
    return 0;
}

_osal_inline_ INT32 osal_bit_op_unlock(P_OSAL_UNSLEEPABLE_LOCK pLock)
{
    
    return 0;
}
#endif
_osal_inline_ INT32 osal_clear_bit(UINT32 bitOffset, P_OSAL_BIT_OP_VAR pData)
{
    osal_bit_op_lock(&(pData->opLock));
    clear_bit(bitOffset, &pData->data);
    osal_bit_op_unlock(&(pData->opLock));
    return 0;
}

_osal_inline_ INT32 osal_set_bit(UINT32 bitOffset, P_OSAL_BIT_OP_VAR pData)
{
    osal_bit_op_lock(&(pData->opLock));
    set_bit(bitOffset, &pData->data);
    osal_bit_op_unlock(&(pData->opLock));
    return 0;
}

_osal_inline_ INT32 osal_test_bit(UINT32 bitOffset, P_OSAL_BIT_OP_VAR pData)
{
    UINT32 iRet = 0;
    osal_bit_op_lock(&(pData->opLock));
    iRet = test_bit(bitOffset, &pData->data);
    osal_bit_op_unlock(&(pData->opLock));
    return iRet;
}

_osal_inline_ INT32 osal_test_and_clear_bit(UINT32 bitOffset, P_OSAL_BIT_OP_VAR pData)
{
    UINT32 iRet = 0;
    osal_bit_op_lock(&(pData->opLock));
    iRet = test_and_clear_bit(bitOffset, &pData->data);
    osal_bit_op_unlock(&(pData->opLock));
    return iRet;

}

_osal_inline_ INT32 osal_test_and_set_bit(UINT32 bitOffset, P_OSAL_BIT_OP_VAR pData)
{
    UINT32 iRet = 0;
    osal_bit_op_lock(&(pData->opLock));
    iRet = test_and_set_bit(bitOffset, &pData->data);
    osal_bit_op_unlock(&(pData->opLock));
    return iRet;
}



INT32 osal_timer_create(P_OSAL_TIMER pTimer)
{
    struct timer_list *timer = &pTimer->timer;
    init_timer(timer);
    timer->function = pTimer->timeoutHandler;
    timer->data = (ULONG)pTimer->timeroutHandlerData;
    return 0;
}
INT32 osal_timer_start(P_OSAL_TIMER pTimer, UINT32 ms)
{

    struct timer_list *timer = &pTimer->timer;
    timer->expires = jiffies + (ms/(1000/HZ));
    add_timer(timer);
    return 0;
}

INT32 osal_timer_stop(P_OSAL_TIMER pTimer)
{
    struct timer_list *timer = &pTimer->timer;
    del_timer(timer);
    return 0;
}

INT32 osal_timer_stop_sync(P_OSAL_TIMER pTimer)
{
    struct timer_list *timer = &pTimer->timer;
    del_timer_sync(timer);
    return 0;
}

INT32 osal_timer_modify(P_OSAL_TIMER pTimer, UINT32 ms)
{

    mod_timer(&pTimer->timer, jiffies + (ms)/(1000/HZ));
    return 0;
}

INT32 _osal_fifo_init(OSAL_FIFO *pFifo, UINT8 *buf, UINT32 size)
{
    struct kfifo *fifo = NULL;
    INT32  ret = -1;

    if(!pFifo || pFifo->pFifoBody)
    {
        printk (KERN_ERR "pFifo must be !NULL, pFifo->pFifoBody must be NULL\n");
        printk (KERN_ERR "pFifo(0x%p), pFifo->pFifoBody(0x%p)\n", pFifo, pFifo->pFifoBody);
        return -1;
    }

	
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
		    spin_lock_init(&pFifo->fifoSpinlock);
		    fifo = kfifo_alloc(size, /*GFP_KERNEL*/GFP_ATOMIC, &pFifo->fifoSpinlock);
			if (NULL == fifo)
			{
			    ret = -2;
			}else
			{
			    ret = 0;
			}
    #else
        fifo = kzalloc(sizeof(struct kfifo), GFP_ATOMIC);
	    if (!buf){
	      /*fifo's buffer is not ready, we allocate automatically*/
	        ret = kfifo_alloc(fifo, size, /*GFP_KERNEL*/GFP_ATOMIC);
	    }else
	    {
            if(is_power_of_2(size))
            {
                kfifo_init(fifo, buf, size);
                ret = 0;
            }
            else
            {
                kfifo_free(fifo);
                fifo = NULL;
                ret = -1;
            }
        } 
    #endif
    pFifo->pFifoBody = fifo;
    return (ret < 0) ? (-1) : (0);
}

INT32 _osal_fifo_deinit(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
        kfifo_free(fifo);
	
    }

    return 0;
}

INT32 _osal_fifo_size(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
        ret = fifo->size;
    #else
        ret = kfifo_size(fifo);
    #endif
    
    }

    return ret;
}

/*returns unused bytes in fifo*/
INT32 _osal_fifo_avail_size(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
        ret = fifo->size - kfifo_len(fifo);
    #else
        ret = kfifo_avail(fifo);
    #endif
    }

    return ret;
}

/*returns used bytes in fifo*/
INT32 _osal_fifo_len(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
        ret = kfifo_len(fifo);
    }

    return ret;
}

INT32 _osal_fifo_is_empty(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
        ret = (fifo->in == fifo->out);
    #else
        ret = kfifo_is_empty(fifo);
    #endif
    }

    return ret;
}

INT32 _osal_fifo_is_full(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
        ret = (fifo->size == _osal_fifo_len(pFifo));
    #else
        ret = kfifo_is_full(fifo);
    #endif
    }

    return ret;
}

INT32 _osal_fifo_data_in(OSAL_FIFO *pFifo, const VOID *buf, UINT32 len)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo && buf && (len <= _osal_fifo_avail_size(pFifo)))
    {       
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
        ret = kfifo_put(fifo, buf, len);
    #else
        ret = kfifo_in(fifo, buf, len);
    #endif
        
    }
    else
    {
        printk("%s: kfifo_in, error, len = %d, _osal_fifo_avail_size = %d, buf=%p\n", 
            __func__, len,  _osal_fifo_avail_size(pFifo), buf);

        ret = 0;
    }

    return ret;
}

INT32 _osal_fifo_data_out(OSAL_FIFO *pFifo, void *buf, UINT32 len)
{
    struct kfifo *fifo = NULL;
    INT32  ret = 0;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    }

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo && buf && (len <= _osal_fifo_len(pFifo)))
    {
    #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
        ret = kfifo_get(fifo, buf, len);
    #else
        ret = kfifo_out(fifo, buf, len);
    #endif
    }
    else
    {
        printk("%s: kfifo_out, error, len = %d, osal_fifo_len = %d, buf=%p\n", 
            __func__, len,  _osal_fifo_len(pFifo), buf);
        
        ret = 0;
    }

    return ret;
}

INT32 _osal_fifo_reset(OSAL_FIFO *pFifo)
{
    struct kfifo *fifo = NULL;

    if(!pFifo || !pFifo->pFifoBody)
    {
        printk("%s:pFifo = NULL or pFifo->pFifoBody = NULL, error\n", __func__);
        return -1;
    } 

    fifo = (struct kfifo *)pFifo->pFifoBody;

    if(fifo)
    {
        kfifo_reset(fifo);
    }

    return 0;
}

INT32 osal_fifo_init(P_OSAL_FIFO pFifo, UINT8 *buffer, UINT32 size)
{
    if(!pFifo)
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return -1;
    }
	
    pFifo->FifoInit = _osal_fifo_init;
    pFifo->FifoDeInit = _osal_fifo_deinit;
    pFifo->FifoSz = _osal_fifo_size;
    pFifo->FifoAvailSz = _osal_fifo_avail_size;
    pFifo->FifoLen = _osal_fifo_len; 
    pFifo->FifoIsEmpty = _osal_fifo_is_empty; 
    pFifo->FifoIsFull = _osal_fifo_is_full; 
    pFifo->FifoDataIn =  _osal_fifo_data_in;
    pFifo->FifoDataOut =  _osal_fifo_data_out;
    pFifo->FifoReset = _osal_fifo_reset;

    if(NULL != pFifo->pFifoBody)
    {
        printk("%s:Becasue pFifo room is avialable, we clear the room and allocate them again.\n", __func__);
        pFifo->FifoDeInit(pFifo->pFifoBody);
		pFifo->pFifoBody = NULL;
    }

    pFifo->FifoInit(pFifo, buffer, size);

    return 0;
}

VOID osal_fifo_deinit(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        pFifo->FifoDeInit(pFifo);
    } 
    else
    {
        printk("%s:pFifo = NULL, error\n", __func__);
    }
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    
#else

    if(pFifo->pFifoBody)
    {
        kfree(pFifo->pFifoBody);
    }
#endif
}

INT32 osal_fifo_reset(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        return pFifo->FifoReset(pFifo);
    }
    else
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return -1;
    }
}

UINT32 osal_fifo_in(P_OSAL_FIFO pFifo, PUINT8 buffer, UINT32 size)
{
    if(pFifo)
    {
        return pFifo->FifoDataIn(pFifo, buffer, size);
    }
    else 
    {   
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

UINT32 osal_fifo_out(P_OSAL_FIFO pFifo, PUINT8 buffer, UINT32 size)
{
    if(pFifo)
    {
        return pFifo->FifoDataOut(pFifo, buffer, size);    
    }
    else
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

UINT32 osal_fifo_len(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        return pFifo->FifoLen(pFifo);
    }
    else 
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

UINT32 osal_fifo_sz(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        return pFifo->FifoSz(pFifo);
    }
    else 
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

UINT32 osal_fifo_avail(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        return pFifo->FifoAvailSz(pFifo);
    }
    else 
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

UINT32 osal_fifo_is_empty(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        return pFifo->FifoIsEmpty(pFifo);
    }
    else 
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

UINT32 osal_fifo_is_full(P_OSAL_FIFO pFifo)
{
    if(pFifo)
    {
        return pFifo->FifoIsFull(pFifo);
    }
    else 
    {
        printk("%s:pFifo = NULL, error\n", __func__);
        return 0;
    }
}

INT32  osal_wake_lock_init(P_OSAL_WAKE_LOCK pLock)
{
    if(!pLock)
    {
        return -1;
    } 
    else 
    {
        wake_lock_init(&pLock->wake_lock, WAKE_LOCK_SUSPEND, pLock->name);
        
        return 0;
    }
}

INT32  osal_wake_lock(P_OSAL_WAKE_LOCK pLock)
{
    if(!pLock)
    {
        return -1;
    }
    else
    {
        wake_lock(&pLock->wake_lock);

        return 0;
    }
}


INT32  osal_wake_unlock(P_OSAL_WAKE_LOCK pLock)
{
    if(!pLock)
    {
        return -1;
    }
    else
    {
        wake_unlock(&pLock->wake_lock);

        return 0;
    }
}

INT32  osal_wake_lock_count(P_OSAL_WAKE_LOCK pLock)
{
    INT32 count = 0;

    if(!pLock)
    {
        return -1;
    }
    else 
    {
        count = wake_lock_active(&pLock->wake_lock);    
        return count;
    }
}


INT32 osal_unsleepable_lock_init (P_OSAL_UNSLEEPABLE_LOCK pUSL)
{
    spin_lock_init(&(pUSL->lock));
    return 0;
}
INT32 osal_lock_unsleepable_lock (P_OSAL_UNSLEEPABLE_LOCK pUSL)
{
    spin_lock_irqsave(&(pUSL->lock), pUSL->flag);
    return 0;
}
INT32 osal_unlock_unsleepable_lock (P_OSAL_UNSLEEPABLE_LOCK pUSL)
{
    spin_unlock_irqrestore(&(pUSL->lock), pUSL->flag);
    return 0;
}

extern INT32 osal_unsleepable_lock_deinit (P_OSAL_UNSLEEPABLE_LOCK pUSL)
{
    return 0;
}


INT32 osal_sleepable_lock_init (P_OSAL_SLEEPABLE_LOCK pSL)
{
    mutex_init (&pSL->lock);
    return 0;
}

INT32 osal_lock_sleepable_lock (P_OSAL_SLEEPABLE_LOCK pSL)
{
   return mutex_lock_interruptible(&pSL->lock);
}

INT32 osal_unlock_sleepable_lock (P_OSAL_SLEEPABLE_LOCK pSL)
{
    mutex_unlock(&pSL->lock);
    return 0;
}


INT32 osal_sleepable_lock_deinit (P_OSAL_SLEEPABLE_LOCK pSL)
{
    mutex_destroy (&pSL->lock);
    return 0;
}

INT32 osal_msleep(UINT32 ms)
{
    msleep(ms);
    return 0;
}

INT32 osal_gettimeofday(PINT32 sec, PINT32 usec)
{
    INT32 ret = 0;
    struct timeval now;

    do_gettimeofday(&now);

    if(sec != NULL)
        *sec = now.tv_sec;
    else
        ret = -1;

    if(usec != NULL)
        *usec = now.tv_usec;
    else
        ret = -1;

    return ret;
}

INT32 osal_printtimeofday(const PUINT8 prefix)
{
    INT32 ret;
    INT32 sec;
    INT32 usec;

    ret = osal_gettimeofday(&sec, &usec);
    ret += osal_dbg_print("%s>sec=%d, usec=%d\n",prefix, sec, usec);

    return ret;
}

VOID
osal_buffer_dump (
    const UINT8 *buf,
    const UINT8 *title,
    const UINT32 len,
    const UINT32 limit
    )
{
    INT32 k;
    UINT32 dump_len;

    printk("start of dump>[%s] len=%d, limit=%d,", title, len, limit);

    dump_len = ((0 != limit) && (len > limit)) ? limit : len;
#if 0
    if(limit != 0)
    {
        len = (len > limit)? (limit) : (len);
    }
#endif

    for (k = 0; k < dump_len ; k++) {
        if((k != 0) && ( k % 16 == 0))  printk("\n");
        printk("0x%02x ",  buf[k]);
    }
    printk("<end of dump\n");
}

UINT32 osal_op_get_id(P_OSAL_OP pOp) 
{
    return (pOp) ? pOp->op.opId : 0xFFFFFFFF;
}

MTK_WCN_BOOL osal_op_is_wait_for_signal(P_OSAL_OP pOp) 
{
    return (pOp && pOp->signal.timeoutValue) ?  MTK_WCN_BOOL_TRUE : MTK_WCN_BOOL_FALSE;
}

VOID osal_op_raise_signal(P_OSAL_OP pOp, INT32 result) 
{
    if (pOp) 
    {
        pOp->result = result;
        osal_raise_signal(&pOp->signal);
    }
}

