/*
 * bzl.c (C) October 2010 by JP Mens
 * Parse the XML produced by a BIND9.x statistics-server to list
 * zones contained therein.
 *
 * Usage: bzl http://127.0.0.1:8053/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/nanohttp.h>

/*
 * Use nano from libxml2 because it's included for free ;-)
 * Switch to libCurl if you prefer.
 */

#define MAXBUF 2048

char *http_fetch(const char *url)
{
        void *ctx;
        int len = 0;
        char *buf = NULL, *ct;

        if (!(ctx = xmlNanoHTTPOpen(url, &ct))) {
                fprintf(stderr, "Can't fetch url %s\n", url);
        } else {
                len = xmlNanoHTTPContentLength(ctx);
                if(len <= 0) len = MAXBUF;
                if ((buf = (char*)malloc(len)) == NULL) {
			xmlNanoHTTPClose(ctx);
			return (NULL);
		}
                len = xmlNanoHTTPRead(ctx,buf,len);
                buf[len] = 0;
                xmlNanoHTTPClose(ctx);
        }
        return buf;
}

char *stdin_fetch(void)
{
	char *buf = NULL;
	size_t size = 1024;
	size_t point = 0;
	size_t n;

	do {
		size = size * 2;
		buf = realloc(buf, size);
		if (buf == NULL) {
			fprintf(stderr, "bzl: malloc: %s\n", strerror(errno));
			return NULL; // space leak
		}
		do {
			n = fread(buf + point, 1, size - point, stdin);
			point += n;
		} while (point < size && n > 0);
	} while (n > 0);
	if (ferror(stdin)) {
		fprintf(stderr, "bzl: read stdin: %s\n", strerror(errno));
		free(buf);
		return NULL;
	}
	return (buf);
}

xmlDocPtr fetchdoc(char *url)
{
        char *buf;
        xmlDocPtr doc;

	if (strcmp(url, "-")) {
		buf = http_fetch(url);
	} else {
		buf = stdin_fetch();
	}

        if (buf == NULL)
                return (NULL);

        doc = xmlParseMemory(buf, strlen(buf));
        if (doc == NULL) {
                fprintf(stderr,"Document not parsed successfully. \n");
                return (NULL);
        }
        return (doc);
}

xmlXPathObjectPtr
getnodeset(xmlDocPtr doc, xmlChar *xpath){

        xmlXPathContextPtr context;
        xmlXPathObjectPtr result;

        context = xmlXPathNewContext(doc);
        result = xmlXPathEvalExpression(xpath, context);
        if (xmlXPathNodeSetIsEmpty(result->nodesetval)){
                printf("No result\n");
                return NULL;
        }
        xmlXPathFreeContext(context);
        return (result);
}

void dozone(xmlDocPtr doc, xmlChar *zone, xmlNodePtr cur, FILE *fp)
{
        char *serial = NULL;

        while (cur != NULL) {
                if (!strcmp(cur->name, "serial"))
                        serial = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                cur = cur->next;
        }

        fprintf(fp, "%s %s\n", serial, zone);

	if (zone)
		xmlFree(zone);
	if (serial)
		xmlFree(serial);
}

int main(int argc, char **argv)
{

        char *url;
        xmlDocPtr doc;
        xmlChar *xpath = (xmlChar *)"/statistics/views/view/zones/zone";
        xmlNodeSetPtr nodeset;
        xmlXPathObjectPtr result;
        int i;

        if (argc != 2) {
                printf("Usage: %s URL\n", argv[0]);
                return(0);
        }

        url = argv[1];
        doc = fetchdoc(url);
        result = getnodeset (doc, xpath);
        if (result) {
                nodeset = result->nodesetval;
                for (i=0; i < nodeset->nodeNr; i++) {
                        xmlNodePtr cur;
                        xmlChar *name;
                        cur = nodeset->nodeTab[i];
                        name = xmlGetProp(cur, "name");
                        if (name != NULL)
                                dozone(doc, name, cur->xmlChildrenNode, stdout);
                }
                xmlXPathFreeObject (result);
        }
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return (0);
}
