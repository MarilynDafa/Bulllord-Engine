/*
Bulllord Game Engine
Copyright (C) 2010-2017 Trix

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include "iapentry.h"
#include "system.h"
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
#include <ppl.h>
#include <ppltasks.h>
#include "xml/ezxml.h"
#include "util.h"
#elif defined BL_PLATFORM_OSX
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#if ! __has_feature(objc_arc)
#error add the -fobjc-arc compiler flag
#endif
#import <StoreKit/StoreKit.h>
#elif defined BL_PLATFORM_IOS
#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>
#if ! __has_feature(objc_arc)
#error add the -fobjc-arc compiler flag
#endif
#elif defined BL_PLATFORM_LINUX
#elif defined BL_PLATFORM_ANDROID
#include <jni.h>
#include <pthread.h>
#include <android/window.h>
#elif defined BL_PLATFORM_WEB
#else
#endif
typedef struct _IAPMember {
	BLAnsi aProductID[128];
    BLEnum eChannel;
#if defined BL_PLATFORM_OSX
    NSMutableArray* pProducts;
    SKPaymentTransaction* pUnVerified;
    BLVoid(*pPurchaseSubscriber)(BLEnum, BLAnsi*, BLAnsi*);
    BLVoid(*pValidationSubscriber)(BLBool);
    BLVoid(*pCheckSubscriber)(BLAnsi*, BLAnsi*, BLAnsi*);
#elif defined BL_PLATFORM_IOS
    NSMutableArray* pProducts;
    SKPaymentTransaction* pUnVerified;
    BLVoid(*pPurchaseSubscriber)(BLEnum, BLAnsi*, BLAnsi*);
    BLVoid(*pValidationSubscriber)(BLBool);
    BLVoid(*pCheckSubscriber)(BLAnsi*, BLAnsi*, BLAnsi*);
#elif defined BL_PLATFORM_ANDROID
	jobject* pActivity;
	JavaVM* pVM;
	JNIEnv* pEnv;
	JNIEnv* pLocalEnv;
	BLVoid(*pPurchaseSubscriber)(BLEnum, BLAnsi*, BLAnsi*);
	BLVoid(*pValidationSubscriber)(BLBool);
	BLVoid(*pCheckSubscriber)(BLAnsi*, BLAnsi*, BLAnsi*);
#endif

}_BLIAPMemberExt;
static _BLIAPMemberExt* _PrIapMem = NULL;
#if defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_OSX)
@interface AppStore : NSObject
@property(nonatomic,copy) NSArray* _availableproducts;
+ (AppStore*)getInstance;
- (void)initiatePaymentRequestForProductWithIdentifier:(NSString*)_ProductID;
- (void)startProductRequest;
- (void)refreshAppStoreReceipt;
@end
@interface AppStore () <SKProductsRequestDelegate, SKPaymentTransactionObserver>
@end
@implementation AppStore
+ (void)initialize{}
+ (AppStore*)getInstance
{
    static AppStore* _shared = NULL;
    if (!_shared)
    {
        static dispatch_once_t _oncepredicate;
        dispatch_once(&_oncepredicate, ^{
            _shared = [[super allocWithZone:nil] init];
            [[SKPaymentQueue defaultQueue] addTransactionObserver:_shared];
        });
    }
    return _shared;
}
+ (id)allocWithZone:(NSZone*)_Zone
{
    return [self getInstance];
}
- (id)copyWithZone:(NSZone*)_Zone
{
    return self;
}
- (void)initiatePaymentRequestForProductWithIdentifier:(NSString*)_ProductID
{
    if (!self._availableproducts)
        NSLog(@"No products are available. Did you initialize MKStoreKit by calling [[MKStoreKit sharedKit] startProductRequest]?");
    if (![SKPaymentQueue canMakePayments])
    {
#if defined(BL_PLATFORM_IOS)
        UIAlertController*_controller = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"In App Purchasing Disabled", @"") message:NSLocalizedString(@"Check your parental control settings and try again later", @"") preferredStyle:UIAlertControllerStyleAlert];

        [[UIApplication sharedApplication].keyWindow.rootViewController
        presentViewController:_controller animated:YES completion:nil];
#else
        NSAlert* _alert = [[NSAlert alloc] init];
        _alert.messageText = NSLocalizedString(@"In App Purchasing Disabled", @"");
        _alert.informativeText = NSLocalizedString(@"Check your parental control settings and try again later", @"");
        [_alert runModal];
#endif
        return;
    }

    [self._availableproducts enumerateObjectsUsingBlock:^(SKProduct* _ThisProduct, NSUInteger _Idx, BOOL* _Stop)
     {
        if ([_ThisProduct.productIdentifier isEqualToString:_ProductID]) {
            *_Stop = YES;
            SKPayment *payment = [SKPayment paymentWithProduct:_ThisProduct];
            [[SKPaymentQueue defaultQueue] addPayment:payment];
        }
    }];
}
- (void)startProductRequest
{
    SKProductsRequest* _request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithArray:_PrIapMem->pProducts]];
    _request.delegate = self;
    [_request start];
}
- (void)productsRequest:(SKProductsRequest*)_Request didReceiveResponse:(SKProductsResponse*)_Response
{
    self._availableproducts = _Response.products;
}
- (void)refreshAppStoreReceipt
{
    SKReceiptRefreshRequest* _refreshrequest = [[SKReceiptRefreshRequest alloc] initWithReceiptProperties:nil];
    _refreshrequest.delegate = self;
    [_refreshrequest start];
}
- (void)requestDidFinish:(SKRequest*)_Request
{
    if([_Request isKindOfClass:[SKReceiptRefreshRequest class]])
    {
        NSURL* _receipturl = [[NSBundle mainBundle] appStoreReceiptURL];
        if ([[NSFileManager defaultManager] fileExistsAtPath:[_receipturl path]])
        {
            NSLog(@"App receipt exists. Preparing to validate and update local stores.");
            [self startValidatingReceiptsAndUpdateLocalStore];
        }
		else
        {
            NSLog(@"Receipt request completed but there is no receipt. The user may have refused to login, or the reciept is missing.");
        }
    }
}
- (void)startValidatingReceiptsAndUpdateLocalStore
{
    [self startValidatingAppStoreReceiptWithCompletionHandler:^(NSArray* _Receipts, NSError* _Error)
    {
        if (_Receipts && !_Error)
        {
            [[NSNotificationCenter defaultCenter] postNotificationName:@"com.mugunthkumar.mkstorekit.validatedreceipts" object:nil];
        }
        else
        {
            [[NSNotificationCenter defaultCenter] postNotificationName:@"com.mugunthkumar.mkstorekit.failedvalidatingreceipts" object:_Error];
        }
    }];
}
- (void)startValidatingAppStoreReceiptWithCompletionHandler:(void (^)(NSArray* _Receipts, NSError* _Error)) _CompletionHandler
{
    NSURL* _receipturl = [[NSBundle mainBundle] appStoreReceiptURL];
    NSError* _receipterror;
    BOOL _ispresent = [_receipturl checkResourceIsReachableAndReturnError:&_receipterror];
    if (!_ispresent)
    {
        _CompletionHandler(nil, nil);
        return;
    }
    NSData* _receiptdata = [NSData dataWithContentsOfURL:_receipturl];
    if (!_receiptdata)
    {
        _CompletionHandler(nil, nil);
        return;
    }
    NSError* _error;
    NSMutableDictionary* _requestcontents = [NSMutableDictionary dictionaryWithObject:
                                            [_receiptdata base64EncodedStringWithOptions:0] forKey:@"receipt-data"];
    NSData* _requestdata = [NSJSONSerialization dataWithJSONObject:_requestcontents options:0 error:&_error];
#ifdef DEBUG
    NSMutableURLRequest* _storerequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"https://sandbox.itunes.apple.com/verifyReceipt"]];
#else
    NSMutableURLRequest* _storerequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"https://buy.itunes.apple.com/verifyReceipt"]];
#endif
    [_storerequest setHTTPMethod:@"POST"];
    [_storerequest setHTTPBody:_requestdata];
    NSURLSession* _session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]];
    [[_session dataTaskWithRequest:_storerequest completionHandler:^(NSData* _Data, NSURLResponse* _Response, NSError* _Error)
    {
        if (!_Error)
        {
            NSDictionary* _jsonresponse = [NSJSONSerialization JSONObjectWithData:_Data options:0 error:&_Error];
            NSInteger _status = [_jsonresponse[@"status"] integerValue];
            if (_jsonresponse[@"receipt"] != [NSNull null])
            {
                NSString* _originalappversion = _jsonresponse[@"receipt"][@"original_application_version"];
                if (nil == _originalappversion)
                {
                    _CompletionHandler(nil, nil);
                    return;
                }
            }
            else
            {
                _CompletionHandler(nil, nil);
                return;
            }
            if (_status != 0)
            {
                _CompletionHandler(nil, [NSError errorWithDomain:@"com.mugunthkumar.mkstorekit" code:_status userInfo:nil]);
                return;
            }
            else
            {
                if (_jsonresponse[@"receipt"] != [NSNull null])
                {
                    NSArray* _inappreceipts = _jsonresponse[@"receipt"][@"in_app"];
                    _CompletionHandler(_inappreceipts, nil);
                    return;
                }
                else
                {
                    _CompletionHandler(nil, nil);
                    return;
                }
            }
        }
        else
        {
            _CompletionHandler(nil, _Error);
            return;
        }
    }] resume];
}
- (void)paymentQueue:(SKPaymentQueue*)_Queue updatedTransactions:(NSArray*)_Transactions
{
    for (SKPaymentTransaction* _transaction in _Transactions)
    {
        switch (_transaction.transactionState) {

            case SKPaymentTransactionStatePurchasing:
                break;
            case SKPaymentTransactionStateDeferred:
                if (_PrIapMem->pPurchaseSubscriber)
                {
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"com.mugunthkumar.mkstorekit.productspurchasedeferred"
                                                                        object:_transaction.payment.productIdentifier];
                }
                break;
            case SKPaymentTransactionStateFailed:
                if (_PrIapMem->pPurchaseSubscriber)
                {
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"com.mugunthkumar.mkstorekit.productspurchasefailed"
                                                                        object:_transaction.payment.productIdentifier];
                }
                else if (_PrIapMem->pCheckSubscriber)
                {
                    _PrIapMem->pCheckSubscriber((BLAnsi*)[_transaction.payment.productIdentifier UTF8String], NULL, NULL);
                    [[SKPaymentQueue defaultQueue] finishTransaction:_transaction];
                }
                break;
            case SKPaymentTransactionStatePurchased:
            case SKPaymentTransactionStateRestored:
            {
                _PrIapMem->pUnVerified = _transaction;
                if (_PrIapMem->pPurchaseSubscriber)
                {
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"com.mugunthkumar.mkstorekit.productspurchased"
                                                                        object:_transaction.payment.productIdentifier];
                }
                else if (_PrIapMem->pCheckSubscriber)
                {
                    NSData* _requestdata = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
                    NSError* _error;
                    NSDictionary* _requestcontents = @{@"receipt-data": [_requestdata base64EncodedStringWithOptions:0]};
                    NSData* _receipt = [NSJSONSerialization dataWithJSONObject:_requestcontents
                                                                       options:0
                                                                         error:&_error];
                    if (!_requestdata)
                    {
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                        _PrIapMem->pCheckSubscriber(NULL, NULL, NULL);
                    }
                    else
                    {
                        NSString* _receiptstr  =[[NSString alloc] initWithData:_receipt encoding:NSUTF8StringEncoding];
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                        strcpy(_PrIapMem->aProductID, (BLAnsi*)[_transaction.payment.productIdentifier UTF8String]);
                        _PrIapMem->pCheckSubscriber((BLAnsi*)[_transaction.payment.productIdentifier UTF8String], (BLAnsi*)[_receiptstr UTF8String], NULL);
                    }
                    _PrIapMem->pCheckSubscriber = NULL;
                }
            }
                break;
        }
    }
}
@end
#elif defined(BL_PLATFORM_ANDROID)
#ifdef __cplusplus
extern "C" {
#endif
BLVoid Jni_productPurchased(JNIEnv* _Env, jobject, jint _Code, jstring _Recept, jstring _Signature)
{
	_PrIapMem->pLocalEnv = _Env;
	const BLUtf8* _recept8 = (const BLUtf8*)_Env->GetStringUTFChars(_Recept, NULL);
	const BLUtf8* _signature8 = (const BLUtf8*)_Env->GetStringUTFChars(_Signature, NULL);
	if (_PrIapMem->pPurchaseSubscriber)
	{
		if (_Code == 0)
			_PrIapMem->pPurchaseSubscriber(BL_PE_SUCCEEDED, (BLAnsi*)_recept8, (BLAnsi*)_signature8);
		else
		{
			_PrIapMem->pPurchaseSubscriber(BL_PE_NOTPURCHASED, NULL, NULL);
			memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
		}
	}
	_Env->ReleaseStringUTFChars(_Signature, (const BLAnsi*)_signature8);
	_Env->ReleaseStringUTFChars(_Recept, (const BLAnsi*)_recept8);
	_PrIapMem->pPurchaseSubscriber = NULL;
	_PrIapMem->pLocalEnv = NULL;
}
BLVoid Jni_validationResult(JNIEnv* _Env, jobject, jint _Code)
{
	_PrIapMem->pLocalEnv = _Env;
	if (_PrIapMem->pValidationSubscriber)
	{
		if (_Code == 0)
			_PrIapMem->pValidationSubscriber(TRUE);
		else
			_PrIapMem->pValidationSubscriber(FALSE);
	}
	memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
	_PrIapMem->pValidationSubscriber = NULL;
	_PrIapMem->pLocalEnv = NULL;
}
BLVoid Jni_checkUnfulfilled(JNIEnv* _Env, jobject, jstring _ProductID, jstring _Recept, jstring _Signature)
{
	_PrIapMem->pLocalEnv = _Env;
	const BLUtf8* _pid8 = (const BLUtf8*)_Env->GetStringUTFChars(_ProductID, NULL);
	const BLUtf8* _recept8 = (const BLUtf8*)_Env->GetStringUTFChars(_Recept, NULL);
	const BLUtf8* _signature8 = (const BLUtf8*)_Env->GetStringUTFChars(_Signature, NULL);
    strcpy(_PrIapMem->aProductID, (const BLAnsi*)_pid8);
	_PrIapMem->pCheckSubscriber((BLAnsi*)_pid8, (BLAnsi*)_recept8, (BLAnsi*)_signature8);
	_Env->ReleaseStringUTFChars(_ProductID, (const BLAnsi*)_pid8);
	_Env->ReleaseStringUTFChars(_Signature, (const BLAnsi*)_signature8);
	_Env->ReleaseStringUTFChars(_Recept, (const BLAnsi*)_recept8);
	_PrIapMem->pCheckSubscriber = NULL;
	_PrIapMem->pLocalEnv = NULL;
}
static JNINativeMethod gMethods[] = {
	{"productPurchased", "(ILjava/lang/String;Ljava/lang/String;)V", (void*)Jni_productPurchased},
	{"validationResult", "(I)V", (void*)Jni_validationResult},
	{"checkUnfulfilled", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void*)Jni_checkUnfulfilled}
};
#ifdef __cplusplus
}
#endif
#endif
BLVoid
blIAPOpenEXT(IN BLAnsi* _Version, ...)
{
    _PrIapMem = (_BLIAPMemberExt*)malloc(sizeof(_BLIAPMemberExt));
	memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
	va_list _argp; 
	BLS32 _argno = 0; 
	BLVoid* _para; 
	va_start(_argp, _Version); 
	while (1) 
	{ 
		_para = va_arg(_argp, void*); 
		_argno++;
#if defined(BL_PLATFORM_WIN32)
		if (_argno == 1);
		else break;
#elif defined(BL_PLATFORM_UWP)
		break;
#elif defined(BL_PLATFORM_LINUX)
		if (_argno == 1);
		else if (_argno == 2);
		else break;
#elif defined(BL_PLATFORM_ANDROID)
		if (_argno == 1)
			_PrIapMem->pVM = (JavaVM*)_para;
		else if(_argno == 2)
			_PrIapMem->pEnv = (JNIEnv*)_para;
		else if (_argno == 3)
			_PrIapMem->pActivity = (jobject*)_para;
		else break;
#elif defined(BL_PLATFORM_OSX)
		if (_argno == 1);
		else break;
#elif defined(BL_PLATFORM_IOS)
		if (_argno == 1);
		else break;
#elif defined(BL_PLATFORM_WEB)
		if (_argno == 1);
		else break;
#endif
	}
	va_end(_argp); 
#if defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_OSX)
    _PrIapMem->pPurchaseSubscriber = NULL;
    _PrIapMem->pValidationSubscriber = NULL;
    _PrIapMem->pCheckSubscriber = NULL;
    _PrIapMem->pUnVerified = NULL;
#elif defined(BL_PLATFORM_ANDROID)
	_PrIapMem->pPurchaseSubscriber = NULL;
	_PrIapMem->pValidationSubscriber = NULL;
	_PrIapMem->pCheckSubscriber = NULL;
	JNIEnv* _env = _PrIapMem->pEnv;
	_PrIapMem->pVM->AttachCurrentThread(&_env, NULL);
	jclass _blcls = _env->GetObjectClass(*_PrIapMem->pActivity);
	_env->RegisterNatives(_blcls, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
	_env->DeleteLocalRef(_blcls);
	_PrIapMem->pVM->DetachCurrentThread();
	_PrIapMem->pLocalEnv = NULL;
#endif
}
BLVoid
blIAPCloseEXT()
{
    free(_PrIapMem);
}
BLVoid
blRegistProductsEXT(IN BLEnum _Channel, IN BLAnsi* _Products, IN BLAnsi* _GoogleLicense)
{
	_PrIapMem->eChannel = _Channel;
#if defined(BL_PLATFORM_IOS) || defined(BL_PLATFORM_OSX)
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
    {
        @autoreleasepool {
            _PrIapMem->pProducts = [NSMutableArray array];
            BLAnsi* _tmp;
            BLAnsi* _products = (BLAnsi*)alloca(strlen(_Products) + 1);
            memset(_products, 0, strlen(_Products) + 1);
            strcpy(_products, _Products);
            _tmp = strtok((BLAnsi*)_products, ",");
            while (_tmp)
            {
                [_PrIapMem->pProducts addObject : [NSString stringWithUTF8String : _tmp]];
                _tmp = strtok(NULL, ",");
            }
            [[AppStore getInstance] startProductRequest];
            [[NSNotificationCenter defaultCenter] addObserverForName:@"com.mugunthkumar.mkstorekit.productspurchased" object:nil queue : [[NSOperationQueue alloc] init] usingBlock : ^ (NSNotification* _Note)
             {
                 NSData* _requestdata = [NSData dataWithContentsOfURL : [[NSBundle mainBundle] appStoreReceiptURL]];
                 NSError* _error;
                 NSDictionary* _requestcontents = @{@"receipt-data": [_requestdata base64EncodedStringWithOptions:0]};
                 NSData* _receipt = [NSJSONSerialization dataWithJSONObject : _requestcontents options : 0 error : &_error];
                 if (!_requestdata && _PrIapMem->pPurchaseSubscriber)
                 {
                     _PrIapMem->pPurchaseSubscriber(BL_PE_UNKNOW, NULL, NULL);
                     _PrIapMem->pPurchaseSubscriber = NULL;
                 }
                 else
                 {
                     NSString* _receiptstr = [[NSString alloc] initWithData:_receipt encoding : NSUTF8StringEncoding];
                     if (_PrIapMem->pPurchaseSubscriber)
                         _PrIapMem->pPurchaseSubscriber(BL_PE_SUCCEEDED, (BLAnsi*)[_receiptstr UTF8String], NULL);
                     _PrIapMem->pPurchaseSubscriber = NULL;
                 }
             }];
            [[NSNotificationCenter defaultCenter] addObserverForName:@"com.mugunthkumar.mkstorekit.productspurchasefailed" object:nil queue : [[NSOperationQueue alloc] init] usingBlock : ^ (NSNotification* _Note)
             {
                 if (_PrIapMem->pPurchaseSubscriber)
                     _PrIapMem->pPurchaseSubscriber(BL_PE_NOTPURCHASED, NULL, NULL);
                 memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                 _PrIapMem->pPurchaseSubscriber = NULL;
             }];
            [[NSNotificationCenter defaultCenter] addObserverForName:@"com.mugunthkumar.mkstorekit.productspurchasedeferred" object:nil queue : [[NSOperationQueue alloc] init] usingBlock : ^ (NSNotification* _Note)
             {
                 if (_PrIapMem->pPurchaseSubscriber)
                     _PrIapMem->pPurchaseSubscriber(BL_PE_NOTFULFILLED, NULL, NULL);
                 memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                 _PrIapMem->pPurchaseSubscriber = NULL;
             }];
            [[NSNotificationCenter defaultCenter] addObserverForName:@"com.mugunthkumar.mkstorekit.failedvalidatingreceipts" object:nil queue : [[NSOperationQueue alloc] init] usingBlock : ^ (NSNotification* _Note)
             {
                 if (_PrIapMem->pValidationSubscriber)
                 {
                     _PrIapMem->pValidationSubscriber(FALSE);
                     if (_PrIapMem->pUnVerified)
                         [[SKPaymentQueue defaultQueue] finishTransaction:_PrIapMem->pUnVerified];
                     _PrIapMem->pUnVerified = NULL;
                     memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                     _PrIapMem->pValidationSubscriber = NULL;
                 }
             }];
            [[NSNotificationCenter defaultCenter] addObserverForName:@"com.mugunthkumar.mkstorekit.subscriptionexpired" object:nil queue : [[NSOperationQueue alloc] init] usingBlock : ^ (NSNotification* _Note)
             {
                 if (_PrIapMem->pValidationSubscriber)
                 {
                     _PrIapMem->pValidationSubscriber(FALSE);
                     if (_PrIapMem->pUnVerified)
                         [[SKPaymentQueue defaultQueue] finishTransaction:_PrIapMem->pUnVerified];
                     _PrIapMem->pUnVerified = NULL;
                     memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                     _PrIapMem->pValidationSubscriber = NULL;
                 }
             }];
            [[NSNotificationCenter defaultCenter] addObserverForName:@"com.mugunthkumar.mkstorekit.validatedreceipts" object:nil queue : [[NSOperationQueue alloc] init] usingBlock : ^ (NSNotification* _Note)
             {
                 if (_PrIapMem->pValidationSubscriber)
                 {
                     _PrIapMem->pValidationSubscriber(TRUE);
                     if (_PrIapMem->pUnVerified)
                         [[SKPaymentQueue defaultQueue] finishTransaction:_PrIapMem->pUnVerified];
                     _PrIapMem->pUnVerified = NULL;
                     memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                     _PrIapMem->pValidationSubscriber = NULL;
                 }
             }];
        }
	}
#elif defined(BL_PLATFORM_ANDROID)
	if (_PrIapMem->eChannel == BL_PC_GOOGLE)	
	{
		BLBool _attached = FALSE;
		JNIEnv* _env = _PrIapMem->pEnv;		
		if (_PrIapMem->pLocalEnv)
			_env = _PrIapMem->pLocalEnv;
		else
		{
			_PrIapMem->pVM->AttachCurrentThread(&_env, NULL);
			_PrIapMem->pLocalEnv = _env;
			_attached = TRUE;
		}
		jclass _blcls = _env->GetObjectClass(*_PrIapMem->pActivity);
		jmethodID _mid = _env->GetMethodID(_blcls, "setSignature", "(Ljava/lang/String;)V");
		jstring _license = _env->NewStringUTF((const BLAnsi*)_GoogleLicense);
		_env->CallVoidMethod(*_PrIapMem->pActivity, _mid, _license);
		_env->DeleteLocalRef(_blcls);
		_env->DeleteLocalRef(_license);
		if (_attached)
			_PrIapMem->pVM->DetachCurrentThread();
		_PrIapMem->pLocalEnv = NULL;
	}
#endif
}
BLBool
blPurchaseEXT(IN BLAnsi* _ProductID, IN BLVoid(*_Subscriber)(BLEnum, BLAnsi*, BLAnsi*))
{
	if (_PrIapMem->aProductID[0] != 0)
		return FALSE;
	strcpy(_PrIapMem->aProductID, _ProductID);
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
	{
		const BLUtf16* _productid16 = blGenUtf16Str((const BLUtf8*)_ProductID);
		Platform::String^ _productid = ref new Platform::String((const wchar_t*)_productid16, blUtf16Length(_productid16));
		blDeleteUtf16Str((BLUtf16*)_productid16);		
		auto _dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher;
		_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([_productid, _Subscriber]() {
			auto _operation = Windows::ApplicationModel::Store::CurrentApp::RequestProductPurchaseAsync(_productid);
			concurrency::create_task(_operation).then([=](concurrency::task< Windows::ApplicationModel::Store::PurchaseResults^> _Task)
			{
				try {
					auto _result = _Task.get();
					switch (_result->Status)
					{
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::NotFulfilled:
                        _Subscriber(BL_PE_NOTFULFILLED, NULL);
                        Windows::ApplicationModel::Store::CurrentApp::ReportProductFulfillment(_productid);
						break;
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::NotPurchased:
                        _Subscriber(BL_PE_NOTPURCHASED, NULL);
                        Windows::ApplicationModel::Store::CurrentApp::ReportProductFulfillment(_productid);
						break;
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::AlreadyPurchased:
                        _Subscriber(BL_PE_ALREADYPURCHASED, NULL);
                        Windows::ApplicationModel::Store::CurrentApp::ReportProductFulfillment(_productid);
						break;
					case  Windows::ApplicationModel::Store::ProductPurchaseStatus::Succeeded:
						{
							concurrency::create_task( Windows::ApplicationModel::Store::CurrentApp::GetProductReceiptAsync(_productid)).then([=](concurrency::task<Platform::String^> _Task)
							{
								try {
									Platform::String^ _receipt = _Task.get();
									BLUtf16* _receipt16 = (BLUtf16*)_receipt->Data();
									const BLUtf8* _receipt8 = blGenUtf8Str(_receipt16);
									_Subscriber(BL_PE_SUCCEEDED, (BLAnsi*)_receipt8, NULL);
									blDeleteUtf8Str((BLUtf8*)_receipt8);
								}
								catch (Platform::Exception^ _E) {
									_Subscriber(BL_PE_UNKNOW, NULL, NULL);
								}
							});
						}
						break;
					default:
						_Subscriber(BL_PE_UNKNOW, NULL, NULL);
						break;
					}
				}
				catch (Platform::Exception^ _E) {
					_Subscriber(BL_PE_UNKNOW, NULL, NULL);
				}
			});
		}));
	}	
#elif defined BL_PLATFORM_OSX
	if (_PrIapMem->eChannel == BL_PC_SELF)
    {
    }
    else
    {
        @autoreleasepool {
            _PrIapMem->pPurchaseSubscriber = (BLVoid(*)(BLEnum, BLAnsi*, BLAnsi*))_Subscriber;
            NSString* _productid = [NSString stringWithUTF8String:_ProductID];
            [[AppStore getInstance] initiatePaymentRequestForProductWithIdentifier:_productid];
        }
    }
#elif defined BL_PLATFORM_IOS
	if (_PrIapMem->eChannel == BL_PC_SELF)
    {
    }
    else
    {
        @autoreleasepool {
            _PrIapMem->pPurchaseSubscriber = (BLVoid(*)(BLEnum, BLAnsi*, BLAnsi*))_Subscriber;
            NSString* _productid = [NSString stringWithUTF8String:_ProductID];
            [[AppStore getInstance] initiatePaymentRequestForProductWithIdentifier:_productid];
        }
    }
#elif defined BL_PLATFORM_LINUX
#elif defined BL_PLATFORM_ANDROID
	if (_PrIapMem->eChannel == BL_PC_GOOGLE)
	{
		_PrIapMem->pPurchaseSubscriber = (BLVoid(*)(BLEnum, BLAnsi*, BLAnsi*))_Subscriber;
		BLBool _attached = FALSE;
		JNIEnv* _env = _PrIapMem->pEnv;
		if (_PrIapMem->pLocalEnv)
			_env = _PrIapMem->pLocalEnv;
		else
		{
			_PrIapMem->pVM->AttachCurrentThread(&_env, NULL);
			_PrIapMem->pLocalEnv = _env;
			_attached = TRUE;
		}
		jclass _blcls = _env->GetObjectClass(*_PrIapMem->pActivity);
		jmethodID _mid = _env->GetMethodID(_blcls, "purchase", "(Ljava/lang/String;)V");
		jstring _productid = _env->NewStringUTF((const BLAnsi*)_ProductID);
		_env->CallVoidMethod(*_PrIapMem->pActivity, _mid, _productid);
		_env->DeleteLocalRef(_blcls);
		_env->DeleteLocalRef(_productid);
		if (_attached)
			_PrIapMem->pVM->DetachCurrentThread();
		_PrIapMem->pLocalEnv = NULL;
	}
#elif defined BL_PLATFORM_WEB
#else
#	"error what's the fucking platform"
#endif
	return TRUE;
}
BLVoid
blValidationEXT(IN BLVoid(*_Subscriber)(BLBool))
{
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
	{
		//MS Store—È÷§
		//https://msdn.microsoft.com/en-us/library/windows/apps/jj649137.aspx
		const BLUtf16* _productid16 = blGenUtf16Str((const BLUtf8*)_PrIapMem->aProductID);
		Platform::String^ _productid = ref new Platform::String((const wchar_t*)_productid16, blUtf16Length(_productid16));
		blDeleteUtf16Str((BLUtf16*)_productid16);
		Windows::ApplicationModel::Store::CurrentApp::ReportProductFulfillment(_productid);
		memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
	}
#elif defined BL_PLATFORM_OSX
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
    {
        @autoreleasepool {
            _PrIapMem->pValidationSubscriber = (BLVoid(*)(BLBool))_Subscriber;
            [[AppStore getInstance] startValidatingReceiptsAndUpdateLocalStore];
        }
	}
#elif defined BL_PLATFORM_IOS
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
    {
        @autoreleasepool {
            _PrIapMem->pValidationSubscriber = (BLVoid(*)(BLBool))_Subscriber;
            [[AppStore getInstance] startValidatingReceiptsAndUpdateLocalStore];
        }
	}
#elif defined BL_PLATFORM_LINUX
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
	{
	}
#elif defined BL_PLATFORM_ANDROID
	if (_PrIapMem->eChannel == BL_PC_GOOGLE)
	{
		_PrIapMem->pValidationSubscriber = (BLVoid(*)(BLBool))_Subscriber;
		BLBool _attached = FALSE;
		JNIEnv* _env = _PrIapMem->pEnv;
		if (_PrIapMem->pLocalEnv)
			_env = _PrIapMem->pLocalEnv;
		else
		{
			_PrIapMem->pVM->AttachCurrentThread(&_env, NULL);
			_PrIapMem->pLocalEnv = _env;
			_attached = TRUE;
		}
		jclass _blcls = _env->GetObjectClass(*_PrIapMem->pActivity);
		jmethodID _mid = _env->GetMethodID(_blcls, "validation", "()V");
		_env->CallVoidMethod(*_PrIapMem->pActivity, _mid);
		_env->DeleteLocalRef(_blcls);
		if (_attached)
			_PrIapMem->pVM->DetachCurrentThread();
		_PrIapMem->pLocalEnv = NULL;
	}
	else
	{
	}
#elif defined BL_PLATFORM_WEB	
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
	{
	}
#else
#	"error what's the fucking platform"
#endif
}
BLVoid 
blCheckUnfulfilledEXT(IN BLVoid(*_Subscriber)(BLAnsi*, BLAnsi*, BLAnsi*))
{
#if defined BL_PLATFORM_WIN32
#elif defined BL_PLATFORM_UWP
	if (_PrIapMem->eChannel == BL_PC_SELF)
	{
	}
	else
	{
		auto _dispatcher = Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher;
		_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([_Subscriber]() 
		{
			concurrency::create_task(Windows::ApplicationModel::Store::CurrentApp::GetUnfulfilledConsumablesAsync()).then([=](concurrency::task<Windows::Foundation::Collections::IVectorView<Windows::ApplicationModel::Store::UnfulfilledConsumable^>^ > _Task)
			{
				try {
					Windows::Foundation::Collections::IVectorView<Windows::ApplicationModel::Store::UnfulfilledConsumable^>^ _unfulfilled = _Task.get();
					for (BLU32 _idx = 0; _idx < _unfulfilled->Size; ++_idx)
					{
						Windows::ApplicationModel::Store::UnfulfilledConsumable^ _un = _unfulfilled->GetAt(_idx);
						concurrency::create_task(Windows::ApplicationModel::Store::CurrentApp::GetProductReceiptAsync(_un->ProductId)).then([=](concurrency::task<Platform::String^> _Task)
						{
							try {							
								const BLUtf8* _productid8 = blGenUtf8Str((const BLUtf16*)_un->ProductId->Data());
								Platform::String^ _receipt = _Task.get();
								BLUtf16* _receipt16 = (BLUtf16*)_receipt->Data();
								const BLUtf8* _receipt8 = blGenUtf8Str(_receipt16);
                                if (blUtf8Length(_receipt8))
                                {			
									memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));					
									strcpy(_PrIapMem->aProductID, (BLAnsi*)_productid8);
									_Subscriber((BLAnsi*)_productid8, (BLAnsi*)_receipt8);
								}
                                else
                                {
                                    _Subscriber((BLAnsi*)_productid8, NULL);
                                    Windows::ApplicationModel::Store::CurrentApp::ReportProductFulfillment(_un->ProductId);
									memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                                }
								blDeleteUtf8Str((BLUtf8*)_productid8);
								blDeleteUtf8Str((BLUtf8*)_receipt8);
							}
							catch (Platform::Exception^ _E) {}
						});
                        break;
					}
				}
				catch (Platform::Exception^ _E) {}
			});
		}));
	}
#elif defined BL_PLATFORM_OSX
    if (_PrIapMem->eChannel == BL_PC_SELF)
    {
    }
    else
    {
        _PrIapMem->pCheckSubscriber = (BLVoid(*)(BLAnsi*, BLAnsi*, BLAnsi*))_Subscriber;
        @autoreleasepool {
            NSArray* _transactions = [SKPaymentQueue defaultQueue].transactions;
            if (_transactions.count > 0)
            {
                SKPaymentTransaction* _transaction = [_transactions firstObject];
                if (_transaction.transactionState == SKPaymentTransactionStatePurchased || _transaction.transactionState == SKPaymentTransactionStateRestored)
                {
                    _PrIapMem->pUnVerified = _transaction;
                    NSData* _requestdata = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
                    NSError* _error;
                    NSDictionary* _requestcontents = @{@"receipt-data": [_requestdata base64EncodedStringWithOptions:0]};
                    NSData* _receipt = [NSJSONSerialization dataWithJSONObject:_requestcontents options:0 error:&_error];
                    if (!_requestdata)
                    {
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                        _Subscriber(NULL, NULL, NULL);
                    }
                    else
                    {
                        NSString* _receiptstr  =[[NSString alloc] initWithData:_receipt encoding:NSUTF8StringEncoding];
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                        strcpy(_PrIapMem->aProductID, (BLAnsi*)[_transaction.payment.productIdentifier UTF8String]);
                        _Subscriber((BLAnsi*)[_transaction.payment.productIdentifier UTF8String], (BLAnsi*)[_receiptstr UTF8String], NULL);
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                    }
                }
                else
                {
                    memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                    _Subscriber((BLAnsi*)[_transaction.payment.productIdentifier UTF8String], NULL, NULL);
                }
            }
        }
    }
#elif defined BL_PLATFORM_IOS
	if (_PrIapMem->eChannel == BL_PC_SELF)
    {
    }
    else
    {
        _PrIapMem->pCheckSubscriber = (BLVoid(*)(BLAnsi*, BLAnsi*, BLAnsi*))_Subscriber;
        @autoreleasepool {
            NSArray* _transactions = [SKPaymentQueue defaultQueue].transactions;
            if (_transactions.count > 0)
            {
                SKPaymentTransaction* _transaction = [_transactions firstObject];
                if (_transaction.transactionState == SKPaymentTransactionStatePurchased || _transaction.transactionState == SKPaymentTransactionStateRestored)
                {
                    _PrIapMem->pUnVerified = _transaction;
                    NSData* _requestdata = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
                    NSError* _error;
                    NSDictionary* _requestcontents = @{@"receipt-data": [_requestdata base64EncodedStringWithOptions:0]};
                    NSData* _receipt = [NSJSONSerialization dataWithJSONObject:_requestcontents options:0 error:&_error];
                    if (!_requestdata)
                    {
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                        _Subscriber(NULL, NULL, NULL);
                    }
                    else
                    {
                        NSString* _receiptstr  =[[NSString alloc] initWithData:_receipt encoding:NSUTF8StringEncoding];
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                        strcpy(_PrIapMem->aProductID, (BLAnsi*)[_transaction.payment.productIdentifier UTF8String]);
                        _Subscriber((BLAnsi*)[_transaction.payment.productIdentifier UTF8String], (BLAnsi*)[_receiptstr UTF8String], NULL);
                        memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                    }
                }
                else
                {
                    memset(_PrIapMem->aProductID, 0, sizeof(_PrIapMem->aProductID));
                    _Subscriber((BLAnsi*)[_transaction.payment.productIdentifier UTF8String], NULL, NULL);
                }
            }
        }
    }
#elif defined BL_PLATFORM_LINUX
#elif defined BL_PLATFORM_ANDROID
	if (_PrIapMem->eChannel == BL_PC_GOOGLE)
	{
		_PrIapMem->pCheckSubscriber = (BLVoid(*)(BLAnsi*, BLAnsi*, BLAnsi*))_Subscriber;
		BLBool _attached = FALSE;
		JNIEnv* _env = _PrIapMem->pEnv;
		if (_PrIapMem->pLocalEnv)
			_env = _PrIapMem->pLocalEnv;
		else
		{
			_PrIapMem->pVM->AttachCurrentThread(&_env, NULL);
			_PrIapMem->pLocalEnv = _env;
			_attached = TRUE;
		}
		jclass _blcls = _env->GetObjectClass(*_PrIapMem->pActivity);
		jmethodID _mid = _env->GetMethodID(_blcls, "checkUnfulfilled", "()V");
		_env->CallVoidMethod(*_PrIapMem->pActivity, _mid);
		_env->DeleteLocalRef(_blcls);
		if (_attached)
			_PrIapMem->pVM->DetachCurrentThread();
		_PrIapMem->pLocalEnv = NULL;
	}
	else
	{
	}
#elif defined BL_PLATFORM_WEB
#else
#	"error what's the fucking platform"
#endif
}
