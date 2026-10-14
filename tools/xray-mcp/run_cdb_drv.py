import json, os, sys
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "server.py")
g = {"__name__": "srvtest", "__file__": p}
exec(compile(open(p, encoding="utf-8").read(), p, "exec"), g)

r = g["t_debug_crash"]({
    "exe": sys.argv[1] if len(sys.argv) > 1 else "AnomalyDX11.exe",
    "args": " ".join(sys.argv[2:]) or "-nointro",
    "timeout_s": 300,
})
print(json.dumps(r, indent=2)[:8000])
