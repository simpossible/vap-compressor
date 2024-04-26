//
//  dictionary.h
//  vap-server
//
//  Created by 梁金锋 on 2024/4/27.
//

#ifndef dictionary_h
#define dictionary_h

typedef struct Dictionary{
}Dictionary;

extern Dictionary *dicCreate(void);

extern void dicSetValue(Dictionary*dic, char *key, void *value);
extern void* dicGetValue(Dictionary*dic, char *key);

#endif /* dictionary_h */
