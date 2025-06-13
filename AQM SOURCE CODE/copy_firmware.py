import shutil
import os

env = DefaultEnvironment()
build_dir = env.subst("$BUILD_DIR")
firmware_path = os.path.join(build_dir, "firmware.bin")
destination_dir = os.path.join(env['PROJECT_DIR'], "firmware")
destination = os.path.join(destination_dir, "firmware.bin")

# Ensure destination exists
os.makedirs(destination_dir, exist_ok=True)

# Copy if the firmware exists
if os.path.exists(firmware_path):
    shutil.copyfile(firmware_path, destination)
    print("✅ Copied firmware.bin to:", destination)
else:
    print("❌ firmware.bin not found at:", firmware_path)
