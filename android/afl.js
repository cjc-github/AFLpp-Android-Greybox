Afl.print(`[*] Starting FRIDA config for PID: ${Process.id}`);


const pStartAddr = DebugSymbol.fromName("fuzz_one_input").address;
Afl.print(`fuzz_one_input: ${pStartAddr}`);

const cm = new CModule(`
  #include <string.h>
  #include <gum/gumdefs.h>

  #define BUF_LEN 256

  void afl_persistent_hook(GumCpuContext *regs, uint8_t *input_buf,
    uint32_t input_buf_len) {

    uint32_t length = (input_buf_len > BUF_LEN) ? BUF_LEN : input_buf_len;
    memcpy((void *)regs->x[0], input_buf, length);
    regs->x[1] = length;
  }
  `,
  {
    memcpy: Module.getExportByName(null, "memcpy")
  }
);



Afl.setPersistentHook(cm.afl_persistent_hook);
Afl.setPersistentAddress(pStartAddr);
Afl.setEntryPoint(pStartAddr);
Afl.setInMemoryFuzzing();
Afl.setInstrumentLibraries();

Afl.print("[*] All done!");
Afl.done();
