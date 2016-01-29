#include <doctotext/doctotext_c_api.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[])
{
	char *input_file, *output_file;
    input_file = argv[1];
    
	DocToTextExtractorParams* params = doctotext_create_extractor_params();
	DocToTextFormattingStyle* formatting = doctotext_create_formatting_style();
	doctotext_extractor_params_set_verbose_logging(params, 0);
	doctotext_extractor_params_set_log_file(params, "");
	doctotext_formatting_style_set_url_style(formatting, DOCTOTEXT_URL_STYLE_EXTENDED);
	doctotext_extractor_params_set_formatting_style(params, formatting);
    
	DocToTextExtractedData* text_data = doctotext_process_file(input_file, params, NULL);    
	DocToTextMetadata* metadata = doctotext_extract_metadata(input_file, params, NULL);
    
    if(text_data)
    {
        const char *text = doctotext_extracted_data_get_text(text_data);
        printf("\ntext\n");
    }
    
	doctotext_free_metadata(metadata);
	doctotext_free_extracted_data(text_data);
    
	doctotext_free_formatting_style(formatting);
	doctotext_free_extractor_params(params);
    
	return 0;
}

