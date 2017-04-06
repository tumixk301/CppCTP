# distutils: language=c++
from libc cimport stdlib
from libcpp.string cimport string
from libc.string cimport strcpy, memset
from SecurityFtdcMdApi cimport *

cdef void MdApi_Release(MdApi self):
    ReleaseMdApi(self.api, self.spi)
    self.api = self.spi = NULL

cdef class MdApi:
    cdef CMdApi *api
    cdef CMdSpi *spi

    def __dealloc__(self):
        MdApi_Release(self)

    def Alive(self):
        if self.spi is not NULL: return self.spi.tid
        if self.api is not NULL: return False

    def Create(self, const_char *pszFlowPath=""):
        if self.api is not NULL: return
        self.api = CreateFtdcMdApi(pszFlowPath)
        CheckMemory(self.api)

    def Release(self):
        MdApi_Release(self)

    def Init(self):
        print(">>> SecurityFtdcMdApi.pyx Init() begin")
        if self.api is NULL or self.spi is not NULL: return
        self.spi = new CMdSpi(<PyObject *>self)
        print(">>> SecurityFtdcMdApi.pyx Init() after create self.spi")
        self.spi.InitPyThread()
        print(">>> SecurityFtdcMdApi.pyx Init() after InitPyThread")
        CheckMemory(self.spi)
        self.api.RegisterSpi(self.spi)
        print(">>> SecurityFtdcMdApi.pyx Init() after RegisterSpi")
        self.api.Init()
        print(">>> SecurityFtdcMdApi.pyx Init() after api Init")

    def Join(self):
        cdef int ret
        if self.spi is NULL: return
        with nogil: ret = self.api.Join()
        return ret

    def GetTradingDay(self):
        cdef const_char *ret
        if self.spi is NULL: return
        with nogil: ret = self.api.GetTradingDay()
        return ret

    def RegisterFront(self, char *pszFrontAddress):
        if self.api is NULL: return
        self.api.RegisterFront(pszFrontAddress)

    def SubscribeMarketData(self, pInstrumentIDs, char *pExchageID):
        cdef int nCount
        cdef char **ppInstrumentID
        if self.spi is NULL: return
        nCount = len(pInstrumentIDs)
        ppInstrumentID = <char **>stdlib.malloc(sizeof(char *) * nCount)
        CheckMemory(ppInstrumentID)
        try:
            for i from 0 <= i < nCount:
                ppInstrumentID[i] = pInstrumentIDs[i]
            with nogil: nCount = self.api.SubscribeMarketData(ppInstrumentID, nCount, pExchageID)
        finally:
            stdlib.free(ppInstrumentID)
        return nCount

    def UnSubscribeMarketData(self, pInstrumentIDs, char *pExchageID):
        cdef int nCount
        cdef char **ppInstrumentID
        if self.spi is NULL: return
        nCount = len(pInstrumentIDs)
        ppInstrumentID = <char **>stdlib.malloc(sizeof(char *) * nCount)
        CheckMemory(ppInstrumentID)
        try:
            for i from 0 <= i < nCount:
                ppInstrumentID[i] = pInstrumentIDs[i]
            with nogil: nCount = self.api.UnSubscribeMarketData(ppInstrumentID, nCount, pExchageID)
        finally:
            stdlib.free(ppInstrumentID)
        return nCount

    def ReqUserLogin(self, pReqUserLogin, int nRequestID):
        if self.spi is NULL: return
        print(">>> SecurityFtdcMdApi.pyx ReqUserLogin", pReqUserLogin, pReqUserLogin['BrokerID'])
        cdef CSecurityFtdcReqUserLoginField loginField
        memset(loginField.UserID, 0, sizeof(loginField.UserID))
        memset(loginField.BrokerID, 0, sizeof(loginField.BrokerID))
        memset(loginField.Password, 0, sizeof(loginField.Password))
        # loginField.BrokerID = pReqUserLogin['BrokerID']
        # loginField.UserID = pReqUserLogin['UserID']
        # loginField.Password = pReqUserLogin['Password']
        strcpy(loginField.BrokerID, pReqUserLogin['BrokerID'])
        strcpy(loginField.UserID, pReqUserLogin['UserID'])
        strcpy(loginField.Password, pReqUserLogin['Password'])

        with nogil: nRequestID = self.api.ReqUserLogin(&loginField, nRequestID)
        return nRequestID

    def ReqUserLogout(self, pUserLogout, int nRequestID):
        if self.spi is NULL: return
        cdef CSecurityFtdcUserLogoutField logoutField
        logoutField.UserID = pUserLogout['UserID']
        logoutField.BrokerID = pUserLogout['BrokerID']
        with nogil: nRequestID = self.api.ReqUserLogout(&logoutField, nRequestID)
        return nRequestID


cdef extern int MdSpi_OnFrontConnected(self) except -1:
    self.OnFrontConnected()
    return 0

cdef extern int MdSpi_OnFrontDisconnected(self, int nReason) except -1:
    self.OnFrontDisconnected(nReason)
    return 0

cdef extern int MdSpi_OnHeartBeatWarning(self, int nTimeLapse) except -1:
    self.OnHeartBeatWarning(nTimeLapse)
    return 0

cdef extern int MdSpi_OnRspError(self, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, cbool bIsLast) except -1:
    dict_pRspInfo = {}
    dict_pRspInfo['ErrorMsg'] = pRspInfo.ErrorMsg
    dict_pRspInfo['ErrorID'] = pRspInfo.ErrorID
    self.OnRspError(dict_pRspInfo, nRequestID, bIsLast)
    return 0

cdef extern int MdSpi_OnRspUserLogin(self, CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, cbool bIsLast) except -1:
    dict_pRspUserLogin = {}
    dict_pRspUserLogin['BrokerID'] = pRspUserLogin.BrokerID
    dict_pRspUserLogin['UserID'] = pRspUserLogin.UserID
    dict_pRspUserLogin['FrontID'] = pRspUserLogin.FrontID
    dict_pRspUserLogin['LoginTime'] = pRspUserLogin.LoginTime
    dict_pRspUserLogin['MaxOrderRef'] = pRspUserLogin.MaxOrderRef
    dict_pRspUserLogin['SessionID'] = pRspUserLogin.SessionID

    dict_pRspInfo = {}
    dict_pRspInfo['ErrorID'] = pRspInfo.ErrorID
    dict_pRspInfo['ErrorMsg'] = pRspInfo.ErrorMsg

    self.OnRspUserLogin(dict_pRspUserLogin, dict_pRspInfo, nRequestID, bIsLast)
    return 0

cdef extern int MdSpi_OnRspUserLogout(self, CSecurityFtdcUserLogoutField *pUserLogout, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, cbool bIsLast) except -1:

    # self.OnRspUserLogout(pUserLogout, pRspInfo, nRequestID, bIsLast)
    return 0

cdef extern int MdSpi_OnRspSubMarketData(self, CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, cbool bIsLast) except -1:
    # self.OnRspSubMarketData(pSpecificInstrument, pRspInfo, nRequestID, bIsLast)
    return 0

cdef extern int MdSpi_OnRspUnSubMarketData(self, CSecurityFtdcSpecificInstrumentField *pSpecificInstrument, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, cbool bIsLast) except -1:
    # self.OnRspUnSubMarketData(pSpecificInstrument, pRspInfo, nRequestID, bIsLast)
    return 0

cdef extern int MdSpi_OnRtnDepthMarketData(self, CSecurityFtdcDepthMarketDataField *pDepthMarketData) except -1:
    # self.OnRtnDepthMarketData(pDepthMarketData)
    return 0