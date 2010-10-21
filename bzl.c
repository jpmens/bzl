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

xmlDocPtr fetchdoc(char *url) 
{ 
        char *buf = http_fetch(url); 
        xmlDocPtr doc; 

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

void dozone(xmlDocPtr doc, xmlNodePtr cur, FILE *fp) 
{ 
        char *zone = NULL, *serial = NULL; 

        while (cur != NULL) { 

                if (!strcmp(cur->name, "name")) { 
                        zone = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1); 
                } else if (!strcmp(cur->name, "serial")) { 
                        serial = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1); 
                } 

                cur = cur->next; 
        } 

        if (zone && serial) { 
                char domain[BUFSIZ], *sp; 

                /* Replace all slashes in the zone name by spaces; 
                 * in a configuration with views, the name contains 
                 * "zone/IN/viewname"; without views "zone/IN". 
                 */ 
                strncpy(domain, zone, sizeof(domain) - 1); 
                for (sp = domain; sp && *sp; sp++) { 
                        if (*sp == '/') 
                                *sp = ' '; 
                } 

                fprintf(fp, "%s %s\n", serial, domain); 
        } 
	if (zone)
		xmlFree(zone);
	if (serial)
		xmlFree(serial);
} 

int main(int argc, char **argv) 
{ 

        char *url; 
        xmlDocPtr doc; 
        xmlChar *xpath = (xmlChar *)"/isc/bind/statistics/views/view/zones/zone"; 
        xmlNodeSetPtr nodeset; 
        xmlXPathObjectPtr result; 
        int i; 
        xmlChar *keyword; 

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
                        cur = nodeset->nodeTab[i]; 

                        dozone(doc, cur->xmlChildrenNode, stdout); 
                } 
                xmlXPathFreeObject (result); 
        } 
        xmlFreeDoc(doc); 
        xmlCleanupParser(); 
        return (0); 
} 
