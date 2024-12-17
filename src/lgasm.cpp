#include <stdio.h>
#include <string.h>

extern "C" {
#include "lg/asm.h"
#include "lg/init.h"
#include "lg/stack.h"
#include "lg/timer.h"
#include "lg/vm.h"
}

#include <vector>
#include <string>

using String = std::string;

//./lgasm --bench 100 fibtail.lga
int main(int argc, const char *argv[]) {
    if(argc == 1){
        String dir = "/media/heaven7/Elements_SE/study/mine/liblg/bench";
        std::vector<String> args = {
            "lgasm",
            "--bench", "100",
            dir + "/fibtail.lga"
        };
        const char* argvs[args.size()];
        for(int i = 0 ; i < (int)args.size() ; i ++){
            argvs[i] = args[i].c_str();
        }
        return main(args.size(), argvs);
    }
  lg_init();
  struct lg_vm vm;
  lg_vm_init(&vm);
  vm.debug = true;
  
  struct lg_stack stack;
  lg_stack_init(&stack);

  uintmax_t bench = 0;
  
  while (--argc && ++argv) {
    const char *a = *argv;

    if (strcmp(a, "--debug") == 0) {
      vm.debug = true;
    } else if (strcmp(a, "--bench") == 0) {
      bench = strtoumax(*(++argv), NULL, 10);
      argc--;
    } else {
      if (!lg_asm(&vm, a)) {
	break;
      }	      
    }
  }

  lg_emit(&vm, LG_INVALID_POS, LG_STOP);
  lg_exec(&vm, &stack, 0);

  if (bench) {
    struct lg_timer t;
    lg_timer_init(&t);
    
    for (int i = 0; i < (int)bench; i++) {
      lg_exec(&vm, &stack, 0);
    }

    printf("%" PRIu64 "us\n", lg_timer_usecs(&t));
  }
  
  if (vm.debug) {
    lg_stack_deinit(&stack);
    lg_vm_deinit(&vm);
    lg_deinit();
  }
  
  return 0;
}
