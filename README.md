# durian

The fourth of many fruits.  Stinky. :stuck_out_tongue_closed_eyes:

A C++11 very opinionated helper library for writing and working with the _Fruits Framework_. 

####Use durian with promises
This is an example of invoking a SOAP action.
```cpp
  shared_ptr<Plustache::Context> vendor::client<socketType>::GetData(const string trackingNumber) {
    // get a login future
    auto login = this->apiClient->methodPromise["POST"];
    // get login template
    auto loginXml = this->generator.render(ctx, "tpl/vendor/Login", "tpl/vendor/root");
    // start the promises
    auto promises = login(servicePath, *loginXml, customHeaders)
      .then(loginResponse(this->ctx))
      .then(getData(this->ctx, "tpl/vendor/GetData", "tpl/vendor/root", trackingNumber))
      .then(getDataResponse())
      .then(logout(this->ctx, "tpl/vendor/Logout", "tpl/vendor/root"))
      .then(logoutResponse());
    //wait
    auto resp = promises.get();
    return ctx;
  }
```

####Use durian with promises and MongoDB
```cpp
  int testMongoPromise() {
    mongoclient::client client("locahost", "27017", "database");

    auto rc = client.connect();

    if (rc) {
      return rc;
    }

    mongo::Query cri(MONGO_QUERY("id" << someId));
    auto b = make_unique<mongo::BSONObj>();
    auto fetch = client.findOnePromise();

    auto promise = fetch("collection", cri, b.get())
      .then([&](boost::future<std::string> fut)->string{
        auto resp = fut.get();
        // do some useful stuff
        return resp;
      })
      .then([&](boost::future<std::string> fut)->string{
        auto resp = fut.get();
        // do more useful stuff
        return resp;
      });
    
    // start and wait for the futures  
    promise.get();

    return 0;
  }
``` 

####Use durian with thunks _(delayed execution by invoking a function that returns a function)_
```cpp
  // test thunks
  {
    typedef std::vector<string> CONTEXT;
    
    auto f = [](CONTEXT v, string* t)->CONTEXT {
      v.push_back(*t);
      return v;
    };

    auto f2 = [](CONTEXT v, string* t, string* t2)->CONTEXT {
      v.push_back(*t);
      v.push_back(*t2);
      return v;
    };

    auto f3 = [](CONTEXT v, string* t, string* t2, string* t3)->CONTEXT {
      v.push_back(*t);
      v.push_back(*t2);
      v.push_back(*t3);
      return v;
    };

    CONTEXT context{ "Hello" };
    string param("World");
    string param2("John");
    string param3("Smith");
    std::ostringstream ss;

    // test 1 parameter argument
    // returns a function f(context, param...)
    auto action = createAction(f);
    
    // dispatch action with 1 parameter
    auto newContext = action(context, param);

    // expect => Hello World
    std::copy(newContext.begin(), newContext.end(), std::ostream_iterator<std::string>(ss, " "));
    cout << ss.str() << endl;

    // test 2 parameter arguments
    // returns a function f(context, param...)
    auto action2 = createAction(f2);

    // dispatch action with 2 parameter
    auto newContext2 = action2(context, param, param2);
    
    // expect => Hello World John
    ss.str("");
    std::copy(newContext2.begin(), newContext2.end(), std::ostream_iterator<std::string>(ss, " "));
    cout << ss.str() << endl;

    // test 3 parameter arguments
    // returns a function f(context, param...)
    auto action3 = createAction(f3);

    // dispatch action with 2 parameter
    auto newContext3 = action3(context, param, param2, param3);

    // expect => Hello World John Smith
    ss.str("");
    std::copy(newContext3.begin(), newContext3.end(), std::ostream_iterator<std::string>(ss, " "));
    cout << ss.str() << endl;
    
    // console should display:
    // Hello World
    // Hello World John
    // Hello World John Smith    
  }
```
####Use durian to write code your coworkers can read
What will your code look like?
```cpp
  template<typename socketType>
  class client : public durian::client<socketType>
  // . . .
  std::map<string, string> customHeaders = { { "Content-Type", "text/xml" } };
  client<SimpleWeb::HTTP> simpleClient("vendorHost:8080", "/someService", "user", "password", customHeaders);
  // get result
  auto resp = simpleClient.GetData("1234");

  // same API for another vendor
  std::map<string, string> customHeaders = { { "Content-Type", "application/json" } };
  // use SSL
  client<SimpleWeb::HTTPS> simpleClient("anotherVendorHost:8080", "/someService", "user", "password", customHeaders);
  // get result
  auto resp = simpleClient.GetOtherData("1234");
```

####Use durian with parallel processing
This example uses the global ```::parallel_for_each``` function in durian to make parallel calls to google search.
```cpp
  int parallelCallExample() {
    auto apiCall = [](shared_ptr<IOIO> o) {
      Semaphore_waiter_notifier w(THREAD_LIMITER);

      SimpleWeb::Client<SimpleWeb::HTTPS> client(o->host);
      auto resp = client.request(o->method, o->path, o->data, o->headers);
      stringstream ss;
      ss << resp->content.rdbuf();
      o->response = ss.str();
    };

    vector<shared_ptr<IOIO>> futures;
    std::map<string, string> headers{ { "foo", "bar" } };

    IOIO ioio1("GET", "www.google.com", "/search?q=apple", "", headers);
    futures.push_back(make_shared<IOIO>(ioio1));

    IOIO ioio2("GET", "www.google.com", "/search?q=orange", "", headers);
    futures.push_back(make_shared<IOIO>(ioio2));

    IOIO ioio3("GET", "www.google.com", "/search?q=banana", "", headers);
    futures.push_back(make_shared<IOIO>(ioio3));

    ::parallel_for_each(futures.begin(), futures.end(), apiCall);

    // get the results
    for (auto& e : futures){
      cout << e->response << endl;
    }
    return 0;
  }
```
