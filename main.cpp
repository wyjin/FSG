void parse_command_line(int argc, char *argv[],vector<string> & files_to_track) {
	static struct option longopts[] = {
		{ "verbose",		no_argument,		nullptr,	'v' },
		{ "files",			required_argument,	nullptr,	'f' },
		{ nullptr,			0,					nullptr,	'\0' }
	};

	int idx = 0;
	char c;
	while ((c = char((getopt_long(argc, argv, "vf:", longopts, &idx)))) != -1) {
		switch (c) {
		case 'v':
			VERBOSE = true;
			break;
		case 'f':
      // TODO: make this work
      files_to_track.push_back(optarg);
			break;
		}
	}
}

int main(int argc, char** argv) {
  vector<string> files_to_track;
  parse_command_line(argc, argv, files_to_track);
  return 0;
}
