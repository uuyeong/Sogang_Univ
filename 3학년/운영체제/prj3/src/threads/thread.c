#include "threads/thread.h"
#include <debug.h>
#include <limits.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "devices/timer.h"
#ifdef USERPROG
#include "userprog/process.h"
#endif

/* Random value for struct thread's `magic' member.
   Used to detect stack overflow.  See the big comment at the top
   of thread.h for details. */
#define THREAD_MAGIC 0xcd6abf4b

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
static struct list ready_list;

/* List of sleeping threads ordered by wakeup tick. */
static struct list sleep_list;

/* Next tick value when a sleeping thread should be woken up. */
static int64_t next_tick_to_awake;

/* List of all processes.  Processes are added to this list
   when they are first scheduled and removed when they exit. */
struct list all_list;

/* Idle thread. */
static struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
static struct thread *initial_thread;

/* Lock used by allocate_tid(). */
static struct lock tid_lock;

/* Stack frame for kernel_thread(). */
struct kernel_thread_frame 
  {
    void *eip;                  /* Return address. */
    thread_func *function;      /* Function to call. */
    void *aux;                  /* Auxiliary data for function. */
  };

/* Statistics. */
static long long idle_ticks;    /* # of timer ticks spent idle. */
static long long kernel_ticks;  /* # of timer ticks in kernel threads. */
static long long user_ticks;    /* # of timer ticks in user programs. */

/* Scheduling. */
#define TIME_SLICE 4            /* # of timer ticks to give each thread. */
static unsigned thread_ticks;   /* # of timer ticks since last yield. */

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
bool thread_mlfqs;

/* Aging flag, enabled via kernel option "-aging". */
bool thread_prior_aging;

/* Aging flag, set via -aging kernel option. */
bool thread_prior_aging;

/* Load average for MLFQS scheduling (fixed-point). */
static int load_avg;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);
static struct thread *running_thread (void);
static struct thread *next_thread_to_run (void);
static void init_thread (struct thread *, const char *name, int priority);
static bool is_thread (struct thread *) UNUSED;
static void *alloc_frame (struct thread *, size_t size);
static void schedule (void);
void thread_schedule_tail (struct thread *prev);
static tid_t allocate_tid (void);

/* External list of all threads */
struct list all_list;

static bool thread_priority_more (const struct list_elem *a,
                                  const struct list_elem *b,
                                  void *aux UNUSED);
static bool thread_donation_more (const struct list_elem *a,
                                  const struct list_elem *b,
                                  void *aux UNUSED);
static bool thread_wakeup_less (const struct list_elem *a,
                                const struct list_elem *b,
                                void *aux UNUSED);
static void thread_donate_priority (struct thread *donee);
static void thread_propagate_donation (struct thread *t);
static void thread_update_priority_internal (struct thread *t);
static void thread_test_preemption_internal (void);
static void thread_aging_update (void);
static void thread_mlfqs_increment (void);
static void thread_mlfqs_update_load_avg_and_recent_cpu (void);
static void thread_mlfqs_update_priorities (void);
static void thread_mlfqs_update_priority_one (struct thread *t, void *aux UNUSED);
static void thread_mlfqs_update_recent_cpu_one (struct thread *t, void *aux UNUSED);

/* Fixed-point helper macros (17.14 format). */
#define FP_SHIFT 14
#define FP_SCALE (1 << FP_SHIFT)
#define INT_TO_FP(n) ((n) * FP_SCALE)
#define FP_TO_INT_ZERO(x) ((x) / FP_SCALE)
#define FP_TO_INT_NEAREST(x) ((x) >= 0 ? ((x) + FP_SCALE / 2) / FP_SCALE \
                                             : ((x) - FP_SCALE / 2) / FP_SCALE)
#define FP_ADD(x, y) ((x) + (y))
#define FP_SUB(x, y) ((x) - (y))
#define FP_ADD_INT(x, n) ((x) + (n) * FP_SCALE)
#define FP_SUB_INT(x, n) ((x) - (n) * FP_SCALE)
#define FP_MUL(x, y) ((int64_t) (x) * (y) / FP_SCALE)
#define FP_MUL_INT(x, n) ((x) * (n))
#define FP_DIV(x, y) ((int64_t) (x) * FP_SCALE / (y))
#define FP_DIV_INT(x, n) ((x) / (n))

/* Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */
void
thread_init (void) 
{
  ASSERT (intr_get_level () == INTR_OFF);

  lock_init (&tid_lock);
  list_init (&ready_list);
  list_init (&all_list);
  list_init (&sleep_list);
  next_tick_to_awake = INT64_MAX;
  load_avg = 0;

  /* Set up a thread structure for the running thread. */
  initial_thread = running_thread ();
  init_thread (initial_thread, "main", PRI_DEFAULT);
  initial_thread->status = THREAD_RUNNING;
  initial_thread->tid = allocate_tid ();
}

/* Starts preemptive thread scheduling by enabling interrupts.
   Also creates the idle thread. */
void
thread_start (void) 
{
  /* Create the idle thread. */
  struct semaphore idle_started;
  sema_init (&idle_started, 0);
  thread_create ("idle", PRI_MIN, idle, &idle_started);

  /* Start preemptive thread scheduling. */
  intr_enable ();

  /* Wait for the idle thread to initialize idle_thread. */
  sema_down (&idle_started);
}

/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void) 
{
  struct thread *t = thread_current ();

  /* Update statistics. */
  if (t == idle_thread)
    idle_ticks++;
#ifdef USERPROG
  else if (t->pagedir != NULL)
    user_ticks++;
#endif
  else
    kernel_ticks++;

  if (thread_mlfqs)
    {
      thread_mlfqs_increment ();

      if (timer_ticks () % TIMER_FREQ == 0)
        thread_mlfqs_update_load_avg_and_recent_cpu ();

      if (timer_ticks () % TIME_SLICE == 0)
        thread_mlfqs_update_priorities ();
    }

  if (thread_prior_aging)
    thread_aging_update ();

  if (!list_empty (&ready_list))
    {
      struct thread *front = list_entry (list_front (&ready_list), struct thread, elem);
      if (front->priority > t->priority)
        intr_yield_on_return ();
    }

  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE)
    intr_yield_on_return ();
}

/* Prints thread statistics. */
void
thread_print_stats (void) 
{
  printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
          idle_ticks, kernel_ticks, user_ticks);
}

static bool
thread_priority_more (const struct list_elem *a,
                      const struct list_elem *b,
                      void *aux UNUSED)
{
  const struct thread *ta = list_entry (a, struct thread, elem);
  const struct thread *tb = list_entry (b, struct thread, elem);
  return ta->priority > tb->priority;
}

static bool
thread_donation_more (const struct list_elem *a,
                      const struct list_elem *b,
                      void *aux UNUSED)
{
  const struct thread *ta = list_entry (a, struct thread, donation_elem);
  const struct thread *tb = list_entry (b, struct thread, donation_elem);
  return ta->priority > tb->priority;
}

static bool
thread_wakeup_less (const struct list_elem *a,
                    const struct list_elem *b,
                    void *aux UNUSED)
{
  const struct thread *ta = list_entry (a, struct thread, elem);
  const struct thread *tb = list_entry (b, struct thread, elem);
  return ta->wakeup_tick < tb->wakeup_tick;
}

void
thread_sleep (int64_t wake_tick)
{
  enum intr_level old_level;
  struct thread *cur = thread_current ();

  ASSERT (!intr_context ());

  old_level = intr_disable ();
  cur->wakeup_tick = wake_tick;
  list_insert_ordered (&sleep_list, &cur->elem, thread_wakeup_less, NULL);
  if (wake_tick < next_tick_to_awake)
    next_tick_to_awake = wake_tick;
  thread_block ();
  intr_set_level (old_level);
}

void
thread_awake (int64_t current_tick)
{
  struct list_elem *e = list_begin (&sleep_list);
  next_tick_to_awake = INT64_MAX;

  while (e != list_end (&sleep_list))
    {
      struct thread *t = list_entry (e, struct thread, elem);
      if (t->wakeup_tick <= current_tick)
        {
          e = list_remove (e);
          thread_unblock (t);
        }
      else
        {
          if (t->wakeup_tick < next_tick_to_awake)
            next_tick_to_awake = t->wakeup_tick;
          break;
        }
    }

  thread_test_preemption ();
}

void
thread_update_priority (struct thread *t)
{
  enum intr_level old_level = intr_disable ();

  thread_update_priority_internal (t);

  if (t->status == THREAD_READY)
    {
      list_remove (&t->elem);
      list_insert_ordered (&ready_list, &t->elem, thread_priority_more, NULL);
    }

  intr_set_level (old_level);
}

void
thread_remove_donations (struct lock *lock)
{
  enum intr_level old_level = intr_disable ();

  if (thread_mlfqs)
    {
      thread_update_priority_internal (thread_current ());
      intr_set_level (old_level);
      return;
    }

  struct list_elem *e = list_begin (&thread_current ()->donations);

  while (e != list_end (&thread_current ()->donations))
    {
      struct thread *donor = list_entry (e, struct thread, donation_elem);
      if (donor->waiting_lock == lock)
        e = list_remove (e);
      else
        e = list_next (e);
    }

  thread_update_priority_internal (thread_current ());
  intr_set_level (old_level);
  thread_test_preemption ();
}

void
thread_test_preemption (void)
{
  enum intr_level old_level = intr_disable ();
  thread_test_preemption_internal ();
  intr_set_level (old_level);
}

void
thread_add_donation (struct thread *donor, struct lock *lock)
{
  if (thread_mlfqs || lock->holder == NULL)
    return;

  enum intr_level old_level = intr_disable ();
  donor->waiting_lock = lock;
  list_insert_ordered (&lock->holder->donations, &donor->donation_elem,
                       thread_donation_more, NULL);
  thread_donate_priority (lock->holder);
  intr_set_level (old_level);
}

static void
thread_update_priority_internal (struct thread *t)
{
  if (thread_mlfqs)
    {
      if (t == idle_thread)
        {
          t->priority = PRI_MIN;
          return;
        }

      int recent = FP_TO_INT_ZERO (FP_DIV_INT (t->recent_cpu, 4));
      int nice_term = t->nice * 2;
      int priority = PRI_MAX - recent - nice_term;
      if (priority > PRI_MAX)
        priority = PRI_MAX;
      if (priority < PRI_MIN)
        priority = PRI_MIN;
      t->priority = priority;
      return;
    }

  int max_priority = t->base_priority;
  if (!list_empty (&t->donations))
    {
      list_sort (&t->donations, thread_donation_more, NULL);
      struct thread *highest = list_entry (list_front (&t->donations),
                                           struct thread, donation_elem);
      if (highest->priority > max_priority)
        max_priority = highest->priority;
    }
  if (max_priority > PRI_MAX)
    max_priority = PRI_MAX;
  if (max_priority < PRI_MIN)
    max_priority = PRI_MIN;
  t->priority = max_priority;
}

static void
thread_donate_priority (struct thread *donee)
{
  thread_update_priority_internal (donee);
  if (donee->status == THREAD_READY)
    {
      list_remove (&donee->elem);
      list_insert_ordered (&ready_list, &donee->elem, thread_priority_more, NULL);
    }
  thread_propagate_donation (donee);
}

static void
thread_propagate_donation (struct thread *t)
{
  int depth = 0;
  struct thread *current = t;

  while (current->waiting_lock != NULL && depth < 8)
    {
      struct lock *lock = current->waiting_lock;
      struct thread *holder = lock->holder;

      if (holder == NULL)
        break;

      list_sort (&holder->donations, thread_donation_more, NULL);
      thread_update_priority_internal (holder);

      if (holder->status == THREAD_READY)
        {
          list_remove (&holder->elem);
          list_insert_ordered (&ready_list, &holder->elem, thread_priority_more, NULL);
        }

      current = holder;
      depth++;
    }
}

static void
thread_test_preemption_internal (void)
{
  if (list_empty (&ready_list))
    return;

  struct thread *front = list_entry (list_front (&ready_list), struct thread, elem);
  if (thread_current () != idle_thread && front->priority > thread_current ()->priority)
    {
      if (intr_context ())
        intr_yield_on_return ();
      else
        thread_yield ();
    }
}

static void
thread_mlfqs_increment (void)
{
  if (thread_current () == idle_thread)
    return;

  thread_current ()->recent_cpu = FP_ADD_INT (thread_current ()->recent_cpu, 1);
}

static void
thread_mlfqs_update_load_avg_and_recent_cpu (void)
{
  int ready_threads = list_size (&ready_list);
  if (thread_current () != idle_thread)
    ready_threads++;

  int load_coeff_59 = FP_DIV_INT (INT_TO_FP (59), 60);
  int load_coeff_1 = FP_DIV_INT (INT_TO_FP (1), 60);

  load_avg = FP_ADD (FP_MUL (load_coeff_59, load_avg),
                     FP_MUL (load_coeff_1, INT_TO_FP (ready_threads)));

  thread_foreach (thread_mlfqs_update_recent_cpu_one, NULL);
}

static void
thread_mlfqs_update_priorities (void)
{
  thread_foreach (thread_mlfqs_update_priority_one, NULL);
  list_sort (&ready_list, thread_priority_more, NULL);
  thread_test_preemption_internal ();
}

static void
thread_aging_update (void)
{
  if (thread_mlfqs)
    return;

  struct list_elem *e = list_begin (&ready_list);

  while (e != list_end (&ready_list))
    {
      struct thread *t = list_entry (e, struct thread, elem);
      struct list_elem *next = list_next (e);

      if (t->base_priority < PRI_MAX)
        t->base_priority++;
      thread_update_priority_internal (t);

      e = next;
    }

  list_sort (&ready_list, thread_priority_more, NULL);
}

static void
thread_mlfqs_update_priority_one (struct thread *t, void *aux UNUSED)
{
  if (t == idle_thread)
    return;
  thread_update_priority_internal (t);
}

static void
thread_mlfqs_update_recent_cpu_one (struct thread *t, void *aux UNUSED)
{
  if (t == idle_thread)
    return;

  int double_load = FP_MUL_INT (load_avg, 2);
  int coefficient = FP_DIV (double_load, FP_ADD_INT (double_load, 1));
  t->recent_cpu = FP_ADD_INT (FP_MUL (coefficient, t->recent_cpu), t->nice);
}

/* Creates a new kernel thread named NAME with the given initial
   PRIORITY, which executes FUNCTION passing AUX as the argument,
   and adds it to the ready queue.  Returns the thread identifier
   for the new thread, or TID_ERROR if creation fails.

   If thread_start() has been called, then the new thread may be
   scheduled before thread_create() returns.  It could even exit
   before thread_create() returns.  Contrariwise, the original
   thread may run for any amount of time before the new thread is
   scheduled.  Use a semaphore or some other form of
   synchronization if you need to ensure ordering.

   The code provided sets the new thread's `priority' member to
   PRIORITY, but no actual priority scheduling is implemented.
   Priority scheduling is the goal of Problem 1-3. */
tid_t
thread_create (const char *name, int priority,
               thread_func *function, void *aux) 
{
  struct thread *t;
  struct kernel_thread_frame *kf;
  struct switch_entry_frame *ef;
  struct switch_threads_frame *sf;
  tid_t tid;

  ASSERT (function != NULL);

  /* Allocate thread. */
  t = palloc_get_page (PAL_ZERO);
  if (t == NULL)
    return TID_ERROR;

  /* Initialize thread. */
  init_thread (t, name, priority);
  if (thread_mlfqs)
    {
      t->nice = thread_current ()->nice;
      t->recent_cpu = thread_current ()->recent_cpu;
      thread_update_priority (t);
    }
  tid = t->tid = allocate_tid ();

  /* Stack frame for kernel_thread(). */
  kf = alloc_frame (t, sizeof *kf);
  kf->eip = NULL;
  kf->function = function;
  kf->aux = aux;

  /* Stack frame for switch_entry(). */
  ef = alloc_frame (t, sizeof *ef);
  ef->eip = (void (*) (void)) kernel_thread;

  /* Stack frame for switch_threads(). */
  sf = alloc_frame (t, sizeof *sf);
  sf->eip = switch_entry;
  sf->ebp = 0;

  /* Add to run queue. */
  thread_unblock (t);
 
  if (t->priority > thread_current ()->priority && thread_current () != idle_thread)
    thread_yield ();
 
  /* Add to parent's children list if this is a user process */
#ifdef USERPROG
  if (t->pagedir != NULL)
    {
      struct thread *parent = thread_current ();
      if (parent->pagedir != NULL)
        {
          list_push_back (&parent->children_threads, &t->child_elem);
        }
    }
#endif

  return tid;
}

/* Puts the current thread to sleep.  It will not be scheduled
   again until awoken by thread_unblock().

   This function must be called with interrupts turned off.  It
   is usually a better idea to use one of the synchronization
   primitives in synch.h. */
void
thread_block (void) 
{
  ASSERT (!intr_context ());
  ASSERT (intr_get_level () == INTR_OFF);

  thread_current ()->status = THREAD_BLOCKED;
  schedule ();
}

/* Transitions a blocked thread T to the ready-to-run state.
   This is an error if T is not blocked.  (Use thread_yield() to
   make the running thread ready.)

   This function does not preempt the running thread.  This can
   be important: if the caller had disabled interrupts itself,
   it may expect that it can atomically unblock a thread and
   update other data. */
void
thread_unblock (struct thread *t) 
{
  enum intr_level old_level;

  ASSERT (is_thread (t));
 
  old_level = intr_disable ();
  ASSERT (t->status == THREAD_BLOCKED);
  list_insert_ordered (&ready_list, &t->elem, thread_priority_more, NULL);
  t->status = THREAD_READY;
  intr_set_level (old_level);
 
  thread_test_preemption ();
}

/* Returns the name of the running thread. */
const char *
thread_name (void) 
{
  return thread_current ()->name;
}

/* Returns the running thread.
   This is running_thread() plus a couple of sanity checks.
   See the big comment at the top of thread.h for details. */
struct thread *
thread_current (void) 
{
  struct thread *t = running_thread ();
  
  /* Make sure T is really a thread.
     If either of these assertions fire, then your thread may
     have overflowed its stack.  Each thread has less than 4 kB
     of stack, so a few big automatic arrays or moderate
     recursion can cause stack overflow. */
  ASSERT (is_thread (t));
  ASSERT (t->status == THREAD_RUNNING);

  return t;
}

/* Returns the running thread's tid. */
tid_t
thread_tid (void) 
{
  return thread_current ()->tid;
}

/* Deschedules the current thread and destroys it.  Never
   returns to the caller. */
void
thread_exit (void) 
{
  ASSERT (!intr_context ());

#ifdef USERPROG
  process_exit ();
#endif

  /* Remove thread from all threads list, set our status to dying,
     and schedule another process.  That process will destroy us
     when it calls thread_schedule_tail(). */
  intr_disable ();
  list_remove (&thread_current()->allelem);
  thread_current ()->status = THREAD_DYING;
  schedule ();
  NOT_REACHED ();
}

/* Yields the CPU.  The current thread is not put to sleep and
   may be scheduled again immediately at the scheduler's whim. */
void
thread_yield (void) 
{
  struct thread *cur = thread_current ();
  enum intr_level old_level;
  
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  if (cur != idle_thread) 
    list_insert_ordered (&ready_list, &cur->elem, thread_priority_more, NULL);
  cur->status = THREAD_READY;
  schedule ();
  intr_set_level (old_level);
}

/* Invoke function 'func' on all threads, passing along 'aux'.
   This function must be called with interrupts off. */
void
thread_foreach (thread_action_func *func, void *aux)
{
  struct list_elem *e;

  ASSERT (intr_get_level () == INTR_OFF);

  for (e = list_begin (&all_list); e != list_end (&all_list);
       e = list_next (e))
    {
      struct thread *t = list_entry (e, struct thread, allelem);
      func (t, aux);
    }
}

/* Sets the current thread's priority to NEW_PRIORITY. */
void
thread_set_priority (int new_priority) 
{
  if (thread_mlfqs)
    return;

  if (new_priority > PRI_MAX)
    new_priority = PRI_MAX;
  if (new_priority < PRI_MIN)
    new_priority = PRI_MIN;

  struct thread *cur = thread_current ();
  cur->base_priority = new_priority;
  thread_update_priority (cur);
  thread_test_preemption ();
}

/* Returns the current thread's priority. */
int
thread_get_priority (void) 
{
  return thread_current ()->priority;
}

/* Sets the current thread's nice value to NICE. */
void
thread_set_nice (int nice) 
{
  if (nice > 20)
    nice = 20;
  else if (nice < -20)
    nice = -20;

  struct thread *cur = thread_current ();
  cur->nice = nice;

  if (thread_mlfqs)
    {
      int double_load = FP_MUL_INT (load_avg, 2);
      int coefficient = FP_DIV (double_load, FP_ADD_INT (double_load, 1));
      cur->recent_cpu = FP_ADD_INT (FP_MUL (coefficient, cur->recent_cpu), nice);

      thread_update_priority (cur);
      thread_test_preemption ();
    }
}

/* Returns the current thread's nice value. */
int
thread_get_nice (void) 
{
  return thread_current ()->nice;
}

/* Returns 100 times the system load average. */
int
thread_get_load_avg (void) 
{
  return FP_TO_INT_NEAREST (FP_MUL_INT (load_avg, 100));
}

/* Returns 100 times the current thread's recent_cpu value. */
int
thread_get_recent_cpu (void) 
{
  return FP_TO_INT_NEAREST (FP_MUL_INT (thread_current ()->recent_cpu, 100));
}

/* Idle thread.  Executes when no other thread is ready to run.

   The idle thread is initially put on the ready list by
   thread_start().  It will be scheduled once initially, at which
   point it initializes idle_thread, "up"s the semaphore passed
   to it to enable thread_start() to continue, and immediately
   blocks.  After that, the idle thread never appears in the
   ready list.  It is returned by next_thread_to_run() as a
   special case when the ready list is empty. */
static void
idle (void *idle_started_ UNUSED) 
{
  struct semaphore *idle_started = idle_started_;
  idle_thread = thread_current ();
  sema_up (idle_started);

  for (;;) 
    {
      /* Let someone else run. */
      intr_disable ();
      thread_block ();

      /* Re-enable interrupts and wait for the next one.

         The `sti' instruction disables interrupts until the
         completion of the next instruction, so these two
         instructions are executed atomically.  This atomicity is
         important; otherwise, an interrupt could be handled
         between re-enabling interrupts and waiting for the next
         one to occur, wasting as much as one clock tick worth of
         time.

         See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
         7.11.1 "HLT Instruction". */
      asm volatile ("sti; hlt" : : : "memory");
    }
}

/* Function used as the basis for a kernel thread. */
static void
kernel_thread (thread_func *function, void *aux) 
{
  
  ASSERT (function != NULL);

  intr_enable ();       /* The scheduler runs with interrupts off. */
  function (aux);       /* Execute the thread function. */
  thread_exit ();       /* If function() returns, kill the thread. */
}

/* Returns the running thread. */
struct thread *
running_thread (void) 
{
  uint32_t *esp;

  /* Copy the CPU's stack pointer into `esp', and then round that
     down to the start of a page.  Because `struct thread' is
     always at the beginning of a page and the stack pointer is
     somewhere in the middle, this locates the curent thread. */
  asm ("mov %%esp, %0" : "=g" (esp));
  return pg_round_down (esp);
}

/* Returns true if T appears to point to a valid thread. */
static bool
is_thread (struct thread *t)
{
  return t != NULL && t->magic == THREAD_MAGIC;
}

/* Does basic initialization of T as a blocked thread named
   NAME. */
static void
init_thread (struct thread *t, const char *name, int priority)
{
  enum intr_level old_level;

  ASSERT (t != NULL);
  ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
  ASSERT (name != NULL);

  memset (t, 0, sizeof *t);
  t->status = THREAD_BLOCKED;
  strlcpy (t->name, name, sizeof t->name);
  t->stack = (uint8_t *) t + PGSIZE;
  t->priority = priority;
  t->base_priority = priority;
  t->wakeup_tick = 0;
  list_init (&t->donations);
  t->waiting_lock = NULL;
  t->nice = 0;
  t->recent_cpu = 0;
  t->magic = THREAD_MAGIC;

#ifdef USERPROG
  t->exit_status = -1;  /* Default exit status for abnormal termination */
  list_init (&t->children_threads);  /* Initialize children list */
  sema_init (&t->child_lock, 0);  /* Initialize child semaphore */
  sema_init (&t->remove_lock, 0);  /* Initialize remove semaphore */
  
  /* Initialize file descriptor table */
  int i;
  for (i = 0; i < 128; i++)
    t->fd_table[i] = NULL;
  t->next_fd = 2;  /* FD 0,1 are reserved for STDIN/STDOUT */
  t->executable = NULL;
  
  /* Initialize process loading */
  t->load_success = false;
  sema_init (&t->load_sema, 0);
#endif

  old_level = intr_disable ();
  list_push_back (&all_list, &t->allelem);
  intr_set_level (old_level);
}

/* Allocates a SIZE-byte frame at the top of thread T's stack and
   returns a pointer to the frame's base. */
static void *
alloc_frame (struct thread *t, size_t size) 
{
  /* Stack data is always allocated in word-size units. */
  ASSERT (is_thread (t));
  ASSERT (size % sizeof (uint32_t) == 0);

  t->stack -= size;
  return t->stack;
}

/* Chooses and returns the next thread to be scheduled.  Should
   return a thread from the run queue, unless the run queue is
   empty.  (If the running thread can continue running, then it
   will be in the run queue.)  If the run queue is empty, return
   idle_thread. */
static struct thread *
next_thread_to_run (void) 
{
  if (list_empty (&ready_list))
    return idle_thread;
  else
    return list_entry (list_pop_front (&ready_list), struct thread, elem);
}

/* Completes a thread switch by activating the new thread's page
   tables, and, if the previous thread is dying, destroying it.

   At this function's invocation, we just switched from thread
   PREV, the new thread is already running, and interrupts are
   still disabled.  This function is normally invoked by
   thread_schedule() as its final action before returning, but
   the first time a thread is scheduled it is called by
   switch_entry() (see switch.S).

   It's not safe to call printf() until the thread switch is
   complete.  In practice that means that printf()s should be
   added at the end of the function.

   After this function and its caller returns, the thread switch
   is complete. */
void
thread_schedule_tail (struct thread *prev)
{
  struct thread *cur = running_thread ();
  
  ASSERT (intr_get_level () == INTR_OFF);

  /* Mark us as running. */
  cur->status = THREAD_RUNNING;

  /* Start new time slice. */
  thread_ticks = 0;

#ifdef USERPROG
  /* Activate the new address space. */
  process_activate ();
#endif

  /* If the thread we switched from is dying, destroy its struct
     thread.  This must happen late so that thread_exit() doesn't
     pull out the rug under itself.  (We don't free
     initial_thread because its memory was not obtained via
     palloc().) */
  if (prev != NULL && prev->status == THREAD_DYING && prev != initial_thread) 
    {
      ASSERT (prev != cur);
      palloc_free_page (prev);
    }
}

/* Schedules a new process.  At entry, interrupts must be off and
   the running process's state must have been changed from
   running to some other state.  This function finds another
   thread to run and switches to it.

   It's not safe to call printf() until thread_schedule_tail()
   has completed. */
static void
schedule (void) 
{
  struct thread *cur = running_thread ();
  struct thread *next = next_thread_to_run ();
  struct thread *prev = NULL;

  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (cur->status != THREAD_RUNNING);
  ASSERT (is_thread (next));

  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
}

/* Returns a tid to use for a new thread. */
static tid_t
allocate_tid (void) 
{
  static tid_t next_tid = 1;
  tid_t tid;

  lock_acquire (&tid_lock);
  tid = next_tid++;
  lock_release (&tid_lock);

  return tid;
}

/* Offset of `stack' member within `struct thread'.
   Used by switch.S, which can't figure it out on its own. */
uint32_t thread_stack_ofs = offsetof (struct thread, stack);
