//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Jaejong Bong
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================

#pragma once

#include <memory>
#include "base/publisher/publisher.h"
#include "config/config_manager.h"
#include "http_server/http_server.h"
#include "http_server/https_server.h"
#include "http_server/interceptors/http_request_interceptors.h"
#include "segment_stream_interceptor.h"
#include "segment_stream_observer.h"

//====================================================================================================
// SegmentStreamServer
//====================================================================================================
class SegmentStreamServer
{
public:
	SegmentStreamServer();
	~SegmentStreamServer() = default;

public:
	bool Start(const ov::SocketAddress &address,
			   std::map<int, std::shared_ptr<HttpServer>> &http_server_manager,
			   const ov::String &app_name,
			   int thread_count,
			   const std::shared_ptr<Certificate> &certificate = nullptr,
			   const std::shared_ptr<Certificate> &chain_certificate = nullptr);

	bool Stop();

	bool AddObserver(const std::shared_ptr<SegmentStreamObserver> &observer);
	bool RemoveObserver(const std::shared_ptr<SegmentStreamObserver> &observer);

	bool Disconnect(const ov::String &app_name, const ov::String &stream_name);

	void SetCrossDomain(const std::vector<cfg::Url> &url_list);

	bool GetMonitoringCollectionData(std::vector<std::shared_ptr<MonitoringCollectionData>> &collections);

	virtual cfg::PublisherType GetPublisherType() = 0;
	virtual std::shared_ptr<SegmentStreamInterceptor> CreateInterceptor() = 0;

protected:
	bool RequestUrlParsing(const ov::String &request_url,
						   ov::String &app_name,
						   ov::String &stream_name,
						   ov::String &file_name,
						   ov::String &file_ext);

	bool ProcessRequest(const std::shared_ptr<HttpResponse> &response,
						const ov::String &request_target,
						const ov::String &origin_url);

	bool SetAllowOrigin(const ov::String &origin_url, const std::shared_ptr<HttpResponse> &response);

	// interface
	virtual void ProcessRequestStream(const std::shared_ptr<HttpResponse> &response,
									  const ov::String &app_name, const ov::String &stream_name,
									  const ov::String &file_name,
									  const ov::String &file_ext) = 0;

	virtual void PlayListRequest(const ov::String &app_name, const ov::String &stream_name,
								 const ov::String &file_name,
								 PlayListType play_list_type,
								 const std::shared_ptr<HttpResponse> &response) = 0;

	virtual void SegmentRequest(const ov::String &app_name, const ov::String &stream_name,
								const ov::String &file_name,
								SegmentType segment_type,
								const std::shared_ptr<HttpResponse> &response) = 0;

	bool UrlExistCheck(const std::vector<ov::String> &url_list, const ov::String &check_url);

protected:
	ov::String _app_name;
	std::shared_ptr<HttpServer> _http_server;
	std::vector<std::shared_ptr<SegmentStreamObserver>> _observers;
	std::vector<ov::String> _cors_urls;  // key : url  value : flag(hls/dash allow flag)
	ov::String _cross_domain_xml;
};
