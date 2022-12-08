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
