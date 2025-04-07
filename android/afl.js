Afl.print(`[*] Starting FRIDA config for PID: ${Process.id}`);

/* Modules to be instrumented by Frida */
const MODULE_WHITELIST = [
  "/data/local/tmp/server",
  "/data/local/tmp/libtest_demo.so",
];


/* Persistent loop start address */
const pPersistentAddr = DebugSymbol.fromName("fuzz_one_input").address;

/* Exclude from instrumentation */
Module.load("libandroid_runtime.so");
new ModuleMap().values().forEach(m => {
  if (!MODULE_WHITELIST.includes(m.name)) {
    Afl.print(`Exclude: ${m.base}-${m.base.add(m.size)} ${m.name}`);
    Afl.addExcludedRange(m.base, m.size);
  }
});

Afl.setEntryPoint(pPersistentAddr);

Afl.setPersistentAddress(pPersistentAddr);
Afl.setPersistentCount(10000);
Afl.setInMemoryFuzzing();
Afl.setInstrumentLibraries();

Afl.done();
Afl.print("[*] All done!");
