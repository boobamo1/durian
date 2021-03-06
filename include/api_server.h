#pragma once

#ifndef API_SERVER_H
#define API_SERVER_H

#include "server_http.hpp"
#include "server_https.hpp"
#include <type_traits>
#include <tuple>

using namespace std;

namespace api {

  template<typename T>
  struct Route {
    string expression;
    string verb;
    std::function<void(shared_ptr<typename SimpleWeb::ServerBase<T>::Response>, shared_ptr<typename SimpleWeb::ServerBase<T>::Request>)> action;
  };

  template<typename T>
  class server {
  public:
    server<T>(int _port, int _threads) : port(_port), threads(_threads) {
      _server = make_shared<SimpleWeb::Server<T>>(port, threads);
    }
    server<T>(int _port, int _threads, const string _certFile, const string _privateKeyFile) : port(_port), threads(_threads), certFile(_certFile), privateKeyFile(_privateKeyFile) {
      if (std::is_same<T, SimpleWeb::HTTP>) {
        _server = make_shared<SimpleWeb::Server<T>>(port, threads);
      }
      else {
        _server = make_shared<SimpleWeb::Server<T>>(port, threads, sertFile, privateKeyFile);
      }
    }
    void makeRoutes(vector<Route<T>>& routes) {
      std::for_each(routes.begin(), routes.end(), [&](const Route<T>& r){
        _server->resource[r.expression][r.verb] = r.action;
      });
      //default
      _server->default_resource["GET"] = [](shared_ptr<SimpleWeb::ServerBase<T>::Response> response, shared_ptr<SimpleWeb::ServerBase<T>::Request> request) {
        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << 0 << "\r\n\r\n";
      };
    }
    void start() {
      _server->start();
    }    
  private:
    int port, threads;
    std::string certFile, privateKeyFile, socketType;
    shared_ptr<SimpleWeb::Server<T>> _server;
    vector<Route<T>> routes;
  };
}

#endif