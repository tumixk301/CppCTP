#ifndef QUANT_CTP_MANAGER_H
#define QUANT_CTP_MANAGER_H

#include <list>
#include <map>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include "Login.h"
#include "MdSpi.h"
#include "TdSpi.h"
#include "DBManager.h"
#include "Trader.h"
#include "Session.h"
#include "PositionDetail.h"
#include "xTradeStruct.h"
#include "Timer.h"
#include "safequeue.h"
#include "ApiCommand.h"
#include <spdlog/spdlog.h>
using namespace spdlog;
using spdlog::logger;

using std::map;


class MdSpi;
class MarketConfig;
class ApiCommand;

class CTP_Manager {

public:
	CTP_Manager();
	///登陆
	//bool CheckIn(Login *login);

	/// trader login
	bool TraderLogin(string traderid, string password, Trader *op);

	/// admin login
	bool AdminLogin(string adminid, string password);

	///订阅行情
	int submarketData(char *instrument[]);

	/// 创建交易对象并且登陆
	User * CreateAccount(User *user, list<Strategy *> *l_strategys);

	/// 创建行情
	MdSpi * CreateMd(string td_frontAddress, string td_broker, string td_user, string td_pass, list<Strategy *> *l_strategys);

	///释放
	void ReleaseAccount(User *user);

	///订阅行情
	void SubmarketData(MdSpi *mdspi, list<string > *l_instrument);

	///取消订阅行情
	void UnSubmarketData(MdSpi *mdspi, string instrumentID, list<string > *l_instrument);

	/// 添加合约
	list<string> * addSubInstrument(string instrumentID, list<string> *l_instrument);

	/// 删除订阅合约
	list<string> * delSubInstrument(string instrumentID, list<string> *l_instrument);

	/// 统计合约数量
	int calInstrument(string instrumentID, list<string> *l_instrument);

	/// 退订合约增加
	list<string> addUnSubInstrument(string instrumentID, list<string> l_instrument);

	/// 得到l_instrument
	list<string> *getL_Instrument();

	/// 得到l_unsubinstrument
	list<string> *getL_UnsubInstrument();

	/// 得到数据库操作对象
	DBManager *getDBManager();

	/// 设置l_trader
	void addTraderToLTrader(string trader);

	/// 更新系统交易状态
	void updateSystemFlag();

	/// 获取trader是否在l_trader里
	bool checkInLTrader(string trader);

	bool getTenMinFlag();
	void setTenMinFlag(bool ten_min_flag);

	bool getOneMinFlag();
	void setOneMinFlag(bool one_min_flag);

	bool getOneSecondFlag();
	void setOneSecondFlag(bool one_second_flag);

	/// 得到l_trader
	list<string> *getL_Trader();

	/// 得到l_obj_trader
	list<Trader *> * getL_Obj_Trader();

	/// 移除元素
	void removeFromLTrader(string trader);

	/// 增加用户期货账户
	void addFuturesToTrader(string traderid, User *user);

	/// 获取期货账户map
	map<string, list<User *> *> getTraderMap();

	/// 返回用户列表
	list<User *> *getL_User();

	/// 得到strategy_list
	list<Strategy *> *getListStrategy();

	/// 得到strategy_list
	list<Strategy *> *getListStrategyYesterday();

	/// 得到行情前置地址
	list<MarketConfig *> *getL_MarketConfig();

	/// 得到下单算法
	list<Algorithm *> * getL_Alg();

	/// 设置strategy_list
	void setListStrategy(list<Strategy *> *l_strategys);

	/// 设置定时器
	void setCalTimer(Timer *cal_timer);

	/// 获取定时器
	Timer *getCalTimer();

	/// 保存strategy_list
	void saveStrategy();

	/// 保存所有策略持仓明细
	void saveAllStrategyPositionDetail();

	/// 保存一个策略持仓明细
	void saveStrategyPositionDetail(Strategy *stg);

	/// 保存一个策略持仓明细修改过的
	void saveStrategyChangedPositionDetail(Strategy *stg);

	/// 设置mdspi
	void setMdSpi(MdSpi *mdspi);

	/// 获得mdspi
	MdSpi *getMdSpi();

	/// 初始化
	bool init(bool is_online = true);

	/// 设置开关
	int getOn_Off();
	void setOn_Off(int on_off);

	/// 设置行情是否登陆完成
	bool getMdLogin();
	void setMdLogin(bool isMdLogin);

	/// 设置ctp仓位是否初始化完成
	bool getCTPFinishedPositionInit();
	void setCTPFinishedPositionInit(bool isCTPFinishedPositionInit);

	/// 处理客户端发来的消息
	static void HandleMessage(int fd, char *msg, CTP_Manager *ctp_m);

	/// 初始化数据发送
	static void InitClientData(int fd, CTP_Manager *ctp_m, string s_TraderID, int i_MsgRef, int i_MsgSrc, string s_UserID = "", string s_StrategyID = "");

	/// 行情断线通知


	/// 交易断线通知


	/// 设置交易日
	void setTradingDay(string trading_day);

	/// 获得交易日
	string getTradingDay();

	/// 初始化策略(策略更新trading_day, 期货账户更新order_ref)
	bool initStrategyAndFutureAccount();

	/// 统计本地order,trade持仓明细
	void initPositionDetailDataFromLocalOrderAndTrade();

	/// 增加SOCKET FD
	void addSocketFD(string user_id, int fd);

	/// 删除SOCKET FD
	void delSocketFD(string user_id, int fd);

	/// 发送行情断线通知
	void sendMarketOffLineMessage(int on_off_status);

	/// 发送交易断线通知
	void sendTradeOffLineMessage(string user_id, int on_off_status);

	/// 初始化昨仓明细
	//bool initYesterdayPositionDetail();

	/// 策略增加后同步到Users
	bool syncStrategyAddToUsers(Strategy *stg);

	/// 策略删除后同步到Users
	bool syncStrategyDeleteToUsers(string traderid, string userid, string strategyid);

	//休盘
	void setIsMarketClose(bool is_market_close);
	bool getIsMarketClose();

	//系统收盘最后5秒内要完成的收尾工作标志位
	void setIsStartEndTask(bool is_start_end_task);
	bool getIsStartEndTask();
	void StrategyIsStartEndTask();

	//已经收盘标志位
	void setIsMarketCloseDone(bool is_market_close_done);
	bool getIsMarketCloseDone();

	//获取属于某交易员所属期货账户
	void getUserListByTraderID(string traderid, CTP_Manager *ctp_m, list<User *> *l_user_trader);

	//// 设置系统xts_logger
	//void setXtsLogger(std::shared_ptr<spdlog::logger> ptr);
	std::shared_ptr<spdlog::logger> getXtsLogger();

	sem_t *getSem_strategy_handler();

	bool getIsClosingSaved();
	void setIsClosingSaved(bool isClosingSaved);

	void thread_queue_Command();

	void addCommand(ApiCommand *command);

private:
	//Login *login;
	list<string> *l_instrument;
	list<string > *l_unsubinstrument;
	list<string> *l_trader;
	list<User *> *l_user;
	list<Trader *> *l_obj_trader;
	list<Algorithm *> *l_alg;
	list<MarketConfig *> *l_marketconfig;
	map<string, list<User *> *> m_trader;
	map<string, list<int> *> m_socket_fds;
	DBManager *dbm;
	list<Strategy *> *l_strategys;
	list<Strategy *> *l_strategys_yesterday;
	MdSpi *mdspi;
	int on_off; //开关
	bool is_market_close; //休盘开始标志
	bool is_start_end_task; //系统收盘最后5秒内要完成的收尾工作标志位
	
	bool is_market_close_done; //已经收盘

	string trading_day;
	bool isClosingSaved;
	bool isMdLogin;
	bool isCTPFinishedPositionInit;
	list<Session *> *l_sessions;
	list<USER_CThostFtdcOrderField *> *l_posdetail;
	list<USER_CThostFtdcOrderField *> *l_posdetail_yesterday;

	list<USER_CThostFtdcTradeField *> *l_posdetail_trade;
	list<USER_CThostFtdcTradeField *> *l_posdetail_trade_yesterday;

	Timer *cal_timer;
	std::vector<std::thread> user_threads;

	static bool ten_min_flag;
	static bool one_min_flag;
	static bool one_second_flag;
	bool system_init_flag;
	std::shared_ptr<spdlog::logger> xts_logger;

	sem_t sem_strategy_handler;			// 信号量,用来保证同一时间只能一个线程操作策略

	// 命令队列
	SafeQueue<ApiCommand *> queue_Command;

	// 记录最后一次发送命令类型
	int last_command_type;
};
#endif