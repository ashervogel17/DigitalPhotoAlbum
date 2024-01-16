int generate_thumbnail(char* input_filename, char* thumbnail_filename);

int generate_medium_image(char* input_filename, char* medium_filename);

int display(char* filename);

int get_user_input_for_rotation_and_write_to_pipe(char* input_filename, int rotate_write_fd);

int get_user_input_for_caption_and_write_to_pipe(char* input_filename, int caption_write_fd);

int perform_rotations(char* thumbnail_filename, char* medium_filename, int rotate_read_fd, int caption_read_fd);