#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <dirent.h>

int login=0;
void sanitize(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src == '.' && *(src+1) == '.') {
            src += 2;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

int handle_request(void *cls, struct MHD_Connection *connection,
                const char *url, const char *method, const char *version,
                const char *upload_data, size_t *upload_data_size, void **con_cls) {

    sanitize((char*)url);

    if (strcmp(url, "/") == 0 && strcmp(method, "GET") == 0) {
        FILE *fp = fopen("index.html", "rb");
        if (fp == NULL) {
            return MHD_NO;  // Failed to open file
        }
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *buf = malloc(fsize);
        fread(buf, fsize, 1, fp);
        fclose(fp);

        struct MHD_Response *response = MHD_create_response_from_buffer(fsize, (void*)buf, MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(response, "Content-Type", "text/html");
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        return ret;
    }

    if (strcmp(url, "/login") == 0 && strcmp(method, "GET") == 0) {
        char user[100] = "\x00";
        char *tmp = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "user");
        if (tmp == NULL) {
            strcpy(user, "guest");
        } else {
            strncpy(user, tmp, sizeof(user) - 1);
            user[sizeof(user) - 1] = '\0';
        }
        char welcomePage[20000]="\x00";
        char temps[20000]="\x00";
        strcat(welcomePage, "<html><head><title>Welcome</title></head><body><h1>Welcome, ");
        sprintf(temps, user);
        strcat(welcomePage,temps);
        strcat(welcomePage,"<ul><li><a href=\"file\">FILE MANAGER</a></li>");
        strcat(welcomePage,"</h1></body></html>");

        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(welcomePage), (void*)welcomePage, MHD_RESPMEM_MUST_COPY);
        MHD_add_response_header(response, "Content-Type", "text/html");
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        login=1;
        return ret;
    }


    if (strcmp(url, "/file") == 0 && strcmp(method, "GET") == 0 && login==1) {

        FILE *fp = fopen("file.html", "rb");
        if (fp == NULL) {
            return MHD_NO;  // Failed to open file
        }
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *buf = malloc(fsize);
        fread(buf, fsize, 1, fp);
        fclose(fp);

        struct MHD_Response *response = MHD_create_response_from_buffer(fsize, (void*)buf, MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(response, "Content-Type", "text/html");
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        return ret;

    } else if (strncmp(url, "/list_files/", 12) == 0 && strcmp(method, "GET") == 0 && login==1) {
        // Return a list of all files in the current directory
        // Each file is a hyperlink that will redirect to the "/view_file" route
        DIR *d;
        struct dirent *dir;
        char *dir_name=NULL;
        dir_name = url + 12;
        
        if(!strcmp(dir_name,"\x00"))
        {
            dir_name="./www";
        }
        d = opendir(dir_name);
        if (d) {
            char fileList[100000] = "";
            while ((dir = readdir(d)) != NULL) {
                strcat(fileList, "<a href=\"/view_file/");
                strcat(fileList, dir->d_name);
                strcat(fileList, "\">");
                strcat(fileList, dir->d_name);
                strcat(fileList, "</a><br>");
            }
            closedir(d);

            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(fileList), (void*)fileList, MHD_RESPMEM_PERSISTENT);
            int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);

            return ret;
        }
        return MHD_NO;
    } else if (strncmp(url, "/view_file/", 11) == 0 && strcmp(method, "GET") == 0 && login==1) {
        // Handle file view
        // The filename should be in the rest of the URL
        const char *filename = url + 11;  // Skip "/view_file/"
        FILE *fp = fopen(filename, "rb");
        if (fp == NULL) {
            return MHD_NO;  // Failed to open file
        }
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *buf = malloc(fsize);
        fread(buf, fsize, 1, fp);
        fclose(fp);

        struct MHD_Response *response = MHD_create_response_from_buffer(fsize, (void*)buf, MHD_RESPMEM_MUST_FREE);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        return ret;
    }

    return MHD_NO;  // Unhandled URL
}







void init() {
    char *flag;
    FILE *file = fopen("flag.txt", "r");
    if (file == NULL) {
        printf("Could not open flag file.\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    flag = malloc(fsize + 1);
    fread(flag, fsize, 1, file);
    fclose(file);

    flag[fsize] = '\0';

    if (remove("flag.txt") == 0) {
        printf("File '%s' deleted successfully.\n", "flag.txt");
    } else {
        printf("Unable to delete the file '%s'.\n", "flag.txt");
    }
}

int main() {
    init();

    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, 8080, NULL, NULL,
                              &handle_request, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        return 1;
    }

    while(1);

    MHD_stop_daemon(daemon);

    return 0;
}
