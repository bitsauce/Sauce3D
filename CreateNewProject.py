import shutil
import distutils.dir_util
import os
import argparse

def replaceInFile(filepath, old_string, new_string):
    """Replaces all occurences of old_string with new_string in a file"""
    with open(filepath, "r+", encoding="utf8") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            lines[i] = lines[i].replace(old_string, new_string)
        f.seek(0)
        f.writelines(lines)
        f.truncate()

def main():
    source_dir = os.path.join("samples", "Minimal")

    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Creates a Sauce3D project")
    parser.add_argument("project_name", help="Project name")
    parser.add_argument("target_dir", help="Project target directory. Project files will be written to \"target_dir/project_name/\"")
    args = parser.parse_args()

    # Setup all target paths
    target_dir = os.path.join(args.target_dir, args.project_name)
    if os.path.isdir(target_dir):
        if input("The target path \"{}\" already exists. Continue? [Y/N] ".format(os.path.abspath(target_dir))).lower() != "y":
            print("No project files were created, exiting")
            return

    print("Creating project under \"{}\"...".format(os.path.abspath(target_dir)))

    # Need access to the .sln, .vcxproj and Main.cpp files for later
    sln_file      = os.path.join(target_dir, args.project_name + ".sln")
    proj_file     = os.path.join(target_dir, "Project", args.project_name + ".vcxproj")
    maincpp_file  = os.path.join(target_dir, "Source", "Main.cpp")

    # Make target directories
    os.makedirs(target_dir, exist_ok=True)
    os.makedirs(os.path.join(target_dir, "Project"), exist_ok=True)

    # Copy solution and project files first
    shutil.copyfile(os.path.join(source_dir, "Minimal.sln"), sln_file)
    shutil.copyfile(os.path.join(source_dir, "Project", "Minimal.vcxproj"), proj_file)

    # Copy the rest of the project files
    folders_to_copy = ["Source", "Assets", "Config"]
    for folder in folders_to_copy:
        source_folder = os.path.join(source_dir, folder)
        target_folder = os.path.join(target_dir, folder)
        distutils.dir_util.copy_tree(source_folder, target_folder)

    # Replace "Minimal" with "project_name" in the .sln, .vcxproj and Main.cpp files
    for file in [sln_file, proj_file, maincpp_file]:
        replaceInFile(file, "Minimal", args.project_name)

    print("Project created!")


# Run main() function
if __name__ == "__main__":
    main()
