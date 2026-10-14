import importlib.util, json, sys, os
sys.argv = ["server.py"]
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "server.py")
src = open(p, encoding="utf-8").read()
g = {"__name__": "srvtest", "__file__": p}
exec(compile(src, p, "exec"), g)
TOOLS = g["TOOLS"]
print("tool count:", len(TOOLS))
print("run_cdb present:", "run_cdb" in TOOLS)
print("cdb finder:", g["_find_cdb"]())
print("PDBDIR:", g["PDBDIR"], g["PDBDIR"].is_dir())
# validate every tool's schema renders without error (catches brace typos)
for name, (fn, desc, schema) in sorted(TOOLS.items()):
    json.dumps(schema)
print("all schemas serialize: OK")
