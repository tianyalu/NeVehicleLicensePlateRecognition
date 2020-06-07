#include "utils.h"

void getFiles(const string folder, vector<string>& result) {
	list<string> subfolders;
	subfolders.push_back(folder);
#ifdef WIN32
	while (!subfolders.empty()) {
		std::string current_folder(subfolders.back());

		if (*(current_folder.end() - 1) != '/') {
			current_folder.append("/*");
		}
		else {
			current_folder.append("*");
		}

		subfolders.pop_back();

		struct _finddata_t file_info;
		auto file_handler = _findfirst(current_folder.c_str(), &file_info);

		while (file_handler != -1) {
			if ((!strcmp(file_info.name, ".") || !strcmp(file_info.name, ".."))) {
				if (_findnext(file_handler, &file_info) != 0) break;
				continue;
			}

			if (file_info.attrib & _A_SUBDIR) {

				std::string folder(current_folder);
				folder.pop_back();
				folder.append(file_info.name);

				subfolders.push_back(folder.c_str());

			}
			else {
				std::string file_path;
				file_path.assign(current_folder.c_str()).pop_back();
				file_path.append(file_info.name);


				result.push_back(file_path);

			}

			if (_findnext(file_handler, &file_info) != 0) break;
		}
		_findclose(file_handler);
	}
#else
	while (!subfolders.empty()) {
		string current_folder(subfolders.back());
		if (*(current_folder.end() - 1) != '/') {
			current_folder.push_back('/');
		}
		DIR* pdir = opendir(current_folder.c_str());

		subfolders.pop_back();

		if (!pdir) {
			continue;
		}

		dirent* dir = NULL;

		while ((dir = readdir(pdir)) != NULL) {
			struct stat st;
			if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
				continue;
			}
			if (!strcmp(dir->d_name, ".DS_Store")) {
				continue;
			}

			std::string file_path;

			file_path.append(current_folder.c_str());
			file_path.append(dir->d_name);

			if (lstat(file_path.c_str(), &st) < 0) {
				continue;
			}

			if (S_ISDIR(st.st_mode)) {
				std::string subfolder(current_folder);
				subfolder.append(dir->d_name);
				subfolders.push_back(subfolder.c_str());
			}
			else {
				result.push_back(file_path);


			}
		}
		closedir(pdir);
	}
#endif
}