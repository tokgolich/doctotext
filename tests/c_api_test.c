#include "../build/doctotext_c_api.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

static void date_to_string(char* dest, const struct tm* date)
{
	if (date->tm_year == 0)
		return;
	strftime(dest, 64, "%Y-%m-%d %H:%M:%S", date);
}

int main(int argc, char* argv[])
{
	char* file_name;
	int verbose = 0;
	if (argc == 2)
		file_name = argv[1];
	else if (argc == 3 && strcmp(argv[1], "--verbose") == 0)
	{
		verbose = 1;
		file_name = argv[2];
	}
	else
		return 1;
	DocToTextExtractorParams* params = doctotext_create_extractor_params();
	DocToTextFormattingStyle* formatting = doctotext_create_formatting_style();
	doctotext_extractor_params_set_verbose_logging(params, verbose);
	doctotext_extractor_params_set_log_file(params, "c_api_test.log.out");
	doctotext_formatting_style_set_url_style(formatting, DOCTOTEXT_URL_STYLE_EXTENDED);
	doctotext_extractor_params_set_formatting_style(params, formatting);
	DocToTextException* exception = NULL;
	DocToTextExtractedData* text_data = doctotext_process_file(file_name, params, &exception);
	doctotext_extractor_params_set_log_file(params, "");
	DocToTextMetadata* metadata = doctotext_extract_metadata(file_name, params, &exception);
	doctotext_free_formatting_style(formatting);
	doctotext_free_extractor_params(params);

	printf("%s\n", doctotext_extracted_data_get_text(text_data));
	size_t links_count = doctotext_extracted_data_get_links_count(text_data);
	if (links_count > 0)
	{
		int i;
		DocToTextLink** links = doctotext_extracted_data_get_links(text_data);
		printf("parsed links:\n");
		for (i = 0; i < links_count; ++i)
		{
			printf("link: %s, %d, %d\n", doctotext_link_get_url(links[i]),
				   doctotext_link_get_link_position(links[i]),
				   strlen(doctotext_link_get_link_text(links[i])));
		}
	}

	size_t attachments_count = doctotext_extracted_data_get_attachments_count(text_data);
	if (attachments_count > 0)
	{
		int i;
		DocToTextAttachment** attachments = doctotext_extracted_data_get_attachments(text_data);
		printf("parsed attachments:\n");
		for (i = 0; i < attachments_count; ++i)
		{
			printf("\nname: %s\n", doctotext_attachment_get_file_name(attachments[i]));
			size_t attachment_fields_count = doctotext_attachment_fields_count(attachments[i]);
			if (attachment_fields_count > 0)
			{
				char** keys = doctotext_attachment_get_keys(attachments[i]);
				int j;
				for (j = 0; j < attachment_fields_count; ++j)
				{
					#warning TODO: If Content-ID is not present in the file, mimetic generates it... and test/Makefile always goes wrong.\
					Maybe we should skip this field?
					if (strcmp(keys[j], "Content-ID") != 0)
						printf("key: %s, value: %s\n", keys[j], doctotext_variant_get_string(doctotext_attachment_get_field(attachments[i], keys[j])));
				}
			}
		}
	}
	doctotext_free_extracted_data(text_data);

	printf("\nauthor: %s\n", doctotext_metadata_author(metadata));
	printf("last modify by: %s\n", doctotext_metadata_last_modify_by(metadata));
	char date[64];
	date_to_string(date, doctotext_metadata_creation_date(metadata));
	printf("creation date: %s\n", date);
	date_to_string(date, doctotext_metadata_last_modification_date(metadata));
	printf("last modify date: %s\n", date);
	printf("page count: %d\n", doctotext_metadata_pages_count(metadata));
	printf("word count: %d\n", doctotext_metadata_words_count(metadata));

	char** keys = doctotext_metadata_get_keys(metadata);
	size_t metadata_fields_count = doctotext_metadata_fields_count(metadata);
	int j;
	for (j = 0; j < metadata_fields_count; ++j)
	{
		//If Content-ID is not present in the file, mimetic generates it... and test/Makefile always goes wrong.
		//Maybe we should skip this field?
		if (strcmp(keys[j], "Content-ID") != 0)
			printf("field: %s, value: %s\n", keys[j], doctotext_variant_get_string(doctotext_metadata_get_field(metadata, keys[j])));
	}

	doctotext_free_metadata(metadata);
	return 0;
}
