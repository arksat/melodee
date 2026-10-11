#!/usr/bin/env python3
"""Bundle the existing Web MIDI updater with a verified local firmware package."""
import hashlib
import json
import shutil
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(root / 'web'))
from make_site import check_package

pkg, out = map(Path, sys.argv[1:3])
raw, product = check_package(pkg)
out.mkdir(parents=True, exist_ok=True)
meta = json.dumps({'product': product, 'sha256': hashlib.sha256(raw).hexdigest()})
html = (root / 'web/emu/installer.html').read_text()
assert html.count('/*META*/') == 1
(out / 'index.html').write_text(html.replace('/*META*/', meta))
shutil.copyfile(pkg, out / 'firmware.fwsc')
for name in ('fm1pkg.js', 'fm1ota.js'):
    shutil.copyfile(root / 'web' / name, out / name)
for name in ('LICENSE', 'LICENSING.md'):
    shutil.copyfile(root / name, out / name)
shutil.copytree(root / 'LICENSES', out / 'LICENSES', dirs_exist_ok=True)
