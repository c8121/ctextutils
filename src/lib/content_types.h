#ifndef CTEXTUTILS_CONTENT_TYPES
#define CTEXTUTILS_CONTENT_TYPES

//See https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types

const char *filename_ext_content_type_map[] = {
        ".aac", "audio/aac",
        ".abw", "application/x-abiword",
        ".arc", "application/x-freearc",
        ".avif", "image/avif",
        ".avi", "video/x-msvideo",
        ".azw", "application/vnd.amazon.ebook",
        ".bin", "application/octet-stream",
        ".bmp", "image/bmp",
        ".bz", "application/x-bzip",
        ".bz2", "application/x-bzip2",
        ".cda", "application/x-cdf",
        ".csh", "application/x-csh",
        ".css", "text/css",
        ".csv", "text/csv",
        ".doc", "application/msword",
        ".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
        ".eot", "application/vnd.ms-fontobject",
        ".epub", "application/epub+zip",
        ".gz", "application/gzip",
        ".gif", "image/gif",
        ".html", "text/html",
        ".html", "text/html",
        ".ico", "image/vnd.microsoft.icon",
        ".ics", "text/calendar",
        ".jar", "application/java-archive",
        ".jpeg, .jpg", "image/jpeg",
        ".js", "text/javascript (Specifications: HTML and RFC 9239)",
        ".json", "application/json",
        ".jsonld", "application/ld+json",
        ".mid", "audio/midi, audio/x-midi",
        ".midi", "audio/midi, audio/x-midi",
        ".mjs", "text/javascript",
        ".mp3", "audio/mpeg",
        ".mp4", "video/mp4",
        ".mpeg", "video/mpeg",
        ".mpkg", "application/vnd.apple.installer+xml",
        ".odp", "application/vnd.oasis.opendocument.presentation",
        ".ods", "application/vnd.oasis.opendocument.spreadsheet",
        ".odt", "application/vnd.oasis.opendocument.text",
        ".oga", "audio/ogg",
        ".ogv", "video/ogg",
        ".ogx", "application/ogg",
        ".opus", "audio/opus",
        ".otf", "font/otf",
        ".png", "image/png",
        ".pdf", "application/pdf",
        ".php", "application/x-httpd-php",
        ".ppt", "application/vnd.ms-powerpoint",
        ".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation",
        ".rar", "application/vnd.rar",
        ".rtf", "application/rtf",
        ".sh", "application/x-sh",
        ".svg", "image/svg+xml",
        ".tar", "application/x-tar",
        ".tif", "image/tiff",
        ".tiff", "image/tiff",
        ".ts", "video/mp2t",
        ".ttf", "font/ttf",
        ".txt", "text/plain",
        ".vsd", "application/vnd.visio",
        ".wav", "audio/wav",
        ".weba", "audio/webm",
        ".webm", "video/webm",
        ".webp", "image/webp",
        ".woff", "font/woff",
        ".woff2", "font/woff2",
        ".xhtml", "application/xhtml+xml",
        ".xls", "application/vnd.ms-excel",
        ".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        ".xml", "application/xml",
        ".xul", "application/vnd.mozilla.xul+xml",
        ".zip", "application/zip",
        ".3gp", "video/3gpp",
        ".3g2", "video/3gpp2",
        ".7z", "application/x-7z-compressed",

        //List termination
        "\0", "\0"
};

/**
 * Find match in filename_ext_content_type_map.
 * @return matching extension name or NULL
 */
const char *get_extension_by_content_type(const char *content_type) {
    if (content_type == NULL)
        return NULL;

    for (int i = 0; *filename_ext_content_type_map[i] != '\0'; i += 2) {
        if (strcasecmp(content_type, filename_ext_content_type_map[i + 1]) == 0)
            return filename_ext_content_type_map[i];
    }

    return NULL;
}


#endif //CTEXTUTILS_CONTENT_TYPES