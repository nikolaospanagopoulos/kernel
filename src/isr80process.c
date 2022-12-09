#include "isr80process.h"
#include "process.h"
#include "string.h"
#include "task.h"
void *isr80hCommand6ProcessLoadStart(struct interruptFrame *frame) {
  void *filenameUserPtr = taskGetTaskItem(taskCurrent(), 0);
  char filename[MAX_PATH];

  int res = copyStringFromTask(taskCurrent(), filenameUserPtr, filename,
                               sizeof(filename));

  if (res < 0) {
    goto out;
  }

  char path[MAX_PATH];
  strcpy(path, "0:/");
  strcpy(path + 3, filename);

  struct process *process = 0;

  res = processLoadSwitch(path, &process);
  if (res < 0) {
    goto out;
  }

  taskSwitch(process->task);
  taskReturn(&process->task->registers);
out:
  return 0;
}
void *isr80hCommand7InvokeSystemCommand(struct interruptFrame *frame) {
  return 0;
}
void *isr80hCommand8GetProgramArguments(struct interruptFrame *frame) {
  struct process *process = taskCurrent()->process;
  struct processArguments *arguments = taskVirtualAddressToPhys(
      taskCurrent(), taskGetTaskItem(taskCurrent(), 0));

  processGetArguments(process, &arguments->argc, &arguments->argv);
  return 0;
};
