#include <doctotext/doctotext_c_api.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _type_cmd
{
    int type;
    char *cmd;
} textract_type_cmd_t;

static textract_type_cmd_t g_textract_typecmd_tbl[] = 
{
    {20001, ".doc"}, {20002, ".xls"}, 
    {20011, ".docx"}, {20012, ".xlsx"}, {20013, ".pptx"}, 
    {23001, ".pdf"}, {10002, ".rtf"}, 
    {3, ".txt"}, {4, ".txt"}, {5, ".txt"}, {6, ".txt"}, 
    {-1, NULL},
};

static DocToTextExtractorParams *g_doctotext_params = NULL;
static DocToTextFormattingStyle *g_doctotext_formatting = NULL;

#define PD_EXTRACT_TEXT_HELP  \
    "Usage: %s [OPTION...] [STRING...]\n\n" \
    "    -h, --help                        display this help and exit\n"\
    "\n"\
    "    -i, --input FILE                  set the input file path\n"\
    "    -o, --output FILE                 set the output file path\n"\
    "    -t, --type NUM                    set the input file type\n"\
    "\n"

static char *progname;     /* used throughout */    
static int opt;
static struct option longopts[] = {
    {"help", 0, NULL, 'h'},
    {"input", 1, NULL, 'i'},
    {"output", 1, NULL, 'o'},
    {"type", 1, NULL, 't'},
    {0,0,0,0}
};

static void pd_extract_text_help(void);
static char *_get_textract_typecmd(int type);
static void _set_doctotext_filetype(int file_type);

int main(int argc, char* argv[])
{
    int ret = 0;
    int helpinfo = 1;
    int file_type = 0;
    char *src = NULL;
    char *dst = NULL;
    int pd_extract_text_ok = 0;
    char cmd[1025];
    char *textract_typecmd = NULL;
    FILE *pp, *fpr;
    char buf[1025];
    
    if ((progname = strrchr(argv[0], '/')) != NULL)
        progname++;
    else
        progname = argv[0];

    while((opt = getopt_long(argc, argv, "hi:o:t:", longopts, NULL)) != -1)
    {
        switch(opt)
        {
            case 't':
                file_type = atoi(optarg);
                break;
            case 'i':
                src = strdup(optarg);
                break;
            case 'o':
                dst = strdup(optarg);
                break;
            case 'h':
                helpinfo++;
                break;
            case '?':
                helpinfo++;
                break;
            default:
                helpinfo++;
                break;
        }
    }

    if(src != NULL && dst != NULL)
    {
        helpinfo = 0;
        g_doctotext_params = doctotext_create_extractor_params();
        if(NULL == g_doctotext_params)
        {
            goto next;
        }
        g_doctotext_formatting = doctotext_create_formatting_style();
        if(NULL == g_doctotext_formatting)
        {
            goto next;
        }
        
        doctotext_extractor_params_set_verbose_logging(g_doctotext_params, 1);
        doctotext_formatting_style_set_url_style(g_doctotext_formatting, DOCTOTEXT_URL_STYLE_UNDERSCORED);
        doctotext_extractor_params_set_formatting_style(g_doctotext_params, g_doctotext_formatting);
        doctotext_extractor_params_set_log_file(g_doctotext_params, "");
        _set_doctotext_filetype(file_type);
        
        DocToTextException* exception = NULL;
        DocToTextExtractedData* text_data = NULL;
        text_data = doctotext_process_file(src, g_doctotext_params, &exception);
        if(exception)
        {
            doctotext_free_exception(exception);
            exception = NULL;
            if(text_data)
            {
                doctotext_free_extracted_data(text_data);
            }
            
            goto next;
        }
        
        if(text_data)
        {
            const char *text = doctotext_extracted_data_get_text(text_data);
            
            FILE *fpw = fopen(dst, "wb");
            fwrite(text, strlen(text), 1, fpw);
            fclose(fpw);
            doctotext_free_extracted_data(text_data);
            pd_extract_text_ok = 1;        
            goto exit;
        }
        goto next;
    }
    else
    {
        helpinfo++;
    }
    
    if(helpinfo)
    {
        pd_extract_text_help();
    }
    
next:    
    textract_typecmd = _get_textract_typecmd(file_type);
    if(textract_typecmd)
    {
        snprintf(cmd, 1024, "textract -s %s -o \"%s\" \"%s\"", textract_typecmd, dst, src);
        pp = popen(cmd, "r");
        if(pp == NULL)
        {
            goto exit;
        }
        else
        {
            while(fgets(buf, 1024, pp))
            {
                if(strncmp(buf, "textract_ok", 11) == 0)
                {
                    pclose(pp);
                    pd_extract_text_ok = 1;
                    goto exit;
                }
            }
        }
        pclose(pp);
    }
    
exit:
    if(g_doctotext_formatting)
    {
        doctotext_free_formatting_style(g_doctotext_formatting);
        g_doctotext_formatting = NULL;
    }
    if(g_doctotext_params)
    {
        doctotext_free_extractor_params(g_doctotext_params);
        g_doctotext_params = NULL;
    }

    if(pd_extract_text_ok)
    {
        printf("\npd_extract_text_ok\n");
    }
    else
    {
        printf("\npd_extract_text_err\n");
    }
    
	return 0;
}

static void pd_extract_text_help(void)
{
    (void)fprintf(stderr, PD_EXTRACT_TEXT_HELP, progname);
    exit(1);
}

static char *_get_textract_typecmd(int type)
{
    int size, i;
    size = sizeof(g_textract_typecmd_tbl) / sizeof(textract_type_cmd_t);
    for(i = 0; i < size; i++)
    {
        if(g_textract_typecmd_tbl[i].type == type)
        {
            return g_textract_typecmd_tbl[i].cmd;
        }
    }
    return NULL;
}

static void _set_doctotext_filetype(int file_type)
{
    if(file_type >= 3 && file_type <= 6)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_HTML);
    }
    else if(file_type >= 20011 && file_type <= 20013)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_ODF_OOXML);
    }
    else if(file_type == 20001)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_DOC);
    }
    else if(file_type == 20002)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_XLS);
    }
    else if(file_type == 20003)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_PPT);
    }
    else if(file_type == 23001)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_PDF);
    }
    else if(file_type == 10002)
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_RTF);
    }
    else
    {
        doctotext_extractor_params_set_parser_type(g_doctotext_params, DOCTOTEXT_PARSER_AUTO);
    }
}


