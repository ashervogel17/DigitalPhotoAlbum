#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int write_html_header(char* html_filename) {
  // Open HTML file in write mode
  FILE* fp = fopen(html_filename, "w");

  if (fp != NULL) {
    // Write lines to HTML file
    fprintf(fp, "<!DOCTYPE html>\n");
    fprintf(fp, "<html lang=\"en\">\n");
    fprintf(fp, "<head>\n");
    fprintf(fp, "  <meta charset=\"UTF-8\">\n");
    fprintf(fp, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fp, "  <title>Photo Album</title>\n");
    fprintf(fp, "</head>\n");
    fprintf(fp, "<body>\n");

    // Close HTML file
    fclose(fp);
    return 0;
  }
  // Handle bad file pointer
  else {
    fprintf(stderr, "Error writing HTML file at %s\n", html_filename);
    return -1;
  }
}

int write_html_footer(char* html_filename) {
  // Open HTML file in append mode
  FILE* fp = fopen(html_filename, "a");

  if (fp != NULL) {
    // Write closing lines to HTML file
    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");

    // Close HTML file
    fclose(fp);
    return 0;
  }
  // Handle bad file pointer
  else {
    fprintf(stderr, "Error appending footer to HTML file at %s", html_filename);
    return -1;
  }
}

int add_html_image(char* html_filename, char* thumbnail, char* medium, char* caption) {
  // Open HTML file in append mode
  FILE* fp = fopen(html_filename, "a");

  if (fp != NULL) {
    // Write new div to HTML with thumbnail that links to medium and caption.
    fprintf(fp, "  <div class='image-container'>\n");
    fprintf(fp, "    <a href=%s><img src=%s></a>\n", medium, thumbnail);
    fprintf(fp, "    <caption>%s</caption>\n", caption);
    fprintf(fp, "  </div>\n");

    // Close HTML file
    fclose(fp);
    return 0;
  }
  // Handle bad file pointer
  else {
    fprintf(stderr, "Error appending image container to HTML file at %s", html_filename);
    return -1;
  }
}