import os
import subprocess
import sys

SHADER_DIR = "shaders"
COMPILER = "glslc"

VALID_EXTENSIONS = ('.vert', '.frag', '.comp', '.geom', '.tesc', '.tese')

def compile_shaders():
    if not os.path.exists(SHADER_DIR):
        print(f"Warning: Shader directory '{SHADER_DIR}' not found. Skipping compilation.")
        return

    # get all shader files in the directory
    shader_files = []
    for root, _, files in os.walk(SHADER_DIR):
        for file in files:
            if file.endswith(VALID_EXTENSIONS):
                shader_files.append(os.path.join(root, file))

    if not shader_files:
        print(f"No shaders found in '{SHADER_DIR}/' to compile.")
        return

    # compile each shader if it's outdated or .spv is missing
    for shader in shader_files:
        spv_file = f"{shader}.spv"
        needs_compilation = True

        # check modification times
        if os.path.exists(spv_file):
            shader_mtime = os.path.getmtime(shader)
            spv_mtime = os.path.getmtime(spv_file)
            
            # ff the binary is newer than the source, skip compilation
            if spv_mtime > shader_mtime:
                needs_compilation = False

        if needs_compilation:
            print(f"Compiling: {shader}  ->  {spv_file}")
            try:
                # run the glslc compiler
                result = subprocess.run(
                    [COMPILER, shader, "-o", spv_file], 
                    capture_output=True, 
                    text=True
                )
                
                # check for compilation errors
                if result.returncode != 0:
                    print(f"\n[GLSLC ERROR] Failed to compile {shader}:", file=sys.stderr)
                    print(result.stderr, file=sys.stderr)
                    sys.exit(1) # exit with an error code
                    
            except FileNotFoundError:
                print(f"\n[ERROR] '{COMPILER}' not found.", file=sys.stderr)
                print("Make sure the Vulkan SDK is installed and 'glslc' is added to your system's PATH.", file=sys.stderr)
                sys.exit(1)
        else:
            print(f"Up to date: {shader}")

if __name__ == "__main__":
    print("--- Starting Shader Compilation ---")
    compile_shaders()
    print("--- Shader Compilation Finished ---")