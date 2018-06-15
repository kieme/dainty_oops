#ifndef _DAINTY_OOPS_CTXT_H_
#define _DAINTY_OOPS_CTXT_H_

#include "dainty_named.h"

namespace dainty
{
namespace oops
{
////////////////////////////////////////////////////////////////////////////////

  enum t_category {
    v_category_unrecoverable = 0,
    v_category_recoverable   = 1,
    v_category_ignore        = 2
  };

  using named::t_bool;
  using named::t_void;
  using named::p_cstr;

  using t_tagid    = named::t_uint16;
  using t_lineno   = named::t_uint16;
  using t_id       = named::t_uint32;
  using t_depth    = named::t_uint16;
  using p_filename = p_cstr;
  using p_ctxt     = named::p_cvoid;

  union t_user {
    typedef t_bool (*p_func1)(t_id);
    typedef t_void (*p_func2)(t_id);

    named::t_uint  uint_;
    named::t_int   int_;
    named::p_void  void_;
    named::p_cvoid cvoid_;
    p_cstr         str_;
    p_func1        func1_;
    p_func2        func2_;

    t_user(named::t_uint  user) : uint_ (user) { }
    t_user(named::t_int   user) : int_  (user) { }
    t_user(named::p_void  user) : void_ (user) { }
    t_user(named::p_cvoid user) : cvoid_(user) { }
    t_user(p_cstr         user) : str_  (user) { }
    t_user(p_func1        user) : func1_(user) { }
    t_user(p_func2        user) : func2_(user) { }
  };

  struct t_data1 {
    t_data1(t_bool owner, t_bool mem) : owner_(owner), mem_(mem), tag_(0) { }

    const t_bool owner_;
    const t_bool mem_;
    t_tagid      tag_;
  };

  struct t_data2 {
    t_data2(t_bool owner, t_bool mem)
      : owner_(owner), mem_(mem), depth_(0), tag_(0),
        set_(false), line_(0), file_(0) { }

    t_data2(t_bool owner, t_bool mem, t_depth depth)
      : owner_(owner), mem_(mem), depth_(depth), tag_(0),
        set_(false), line_(0), file_(0) { }

    const t_bool  owner_;
    const t_bool  mem_;
    const t_depth depth_;
    t_tagid       tag_;
    t_bool        set_;
    t_lineno      line_;
    p_filename    file_;
  };

  struct t_def {
    t_category category_;
    p_cstr     string_;
    t_id       next_;
    t_user     user_;

    t_def(t_category category, p_cstr string, t_id next = 0,
          t_user user = t_user(0))
      : category_(category), string_(string), next_(next), user_(user)
    { }
  };

  typedef t_def (*p_what)(t_id);

////////////////////////////////////////////////////////////////////////////////

  struct t_info {
    t_info(p_ctxt ctxt)
      : ctxt_(ctxt), id_(0), what_(0), depth_(0), tag_(0), file_(0), line_(0)
    { }

    inline t_info& set(t_id id, p_what what, t_depth depth,
                       t_tagid tag, p_filename file, t_lineno line) {
      id_    = id;
      what_  = what;
      depth_ = depth;
      tag_   = tag;
      file_  = file;
      line_  = line;
      return *this;
    }

    inline t_info& reset() {
      return set(0, 0, 0, 0, 0, 0);
    }

    p_ctxt     ctxt_;
    t_id       id_;
    p_what     what_;
    t_depth    depth_;
    t_tagid    tag_;
    p_filename file_;
    t_lineno   line_;
  };

////////////////////////////////////////////////////////////////////////////////

  typedef t_void (*p_policy)(const t_info&);
  typedef t_void (*p_print1)(const t_info&, const t_data1&);
  typedef t_void (*p_print2)(const t_info&, const t_data2&);

////////////////////////////////////////////////////////////////////////////////

  t_def  default_what  (t_id);
  t_void default_policy(const t_info&);
  t_void default_print (const t_info&, const t_data1&);
  t_void default_print (const t_info&, const t_data2&);

  t_void trace_step_in  (const t_info&, p_what, p_ctxt, const t_data1&);
  t_void trace_step_in  (const t_info&, p_what, p_ctxt, const t_data2&);
  t_void trace_step_out (const t_info&, p_what, p_ctxt, const t_data1&);
  t_void trace_step_out (const t_info&, p_what, p_ctxt, const t_data2&);
  t_void trace_step_do  (const t_info&, p_what, p_ctxt, const t_data1&);
  t_void trace_step_do  (const t_info&, p_what, p_ctxt, const t_data2&);

////////////////////////////////////////////////////////////////////////////////

#ifdef OOPS_BASIC
  using t_data  = t_data1;
  using p_print = p_print1;
#else
  using t_data  = t_data2;
  using p_print = p_print2;
#endif

////////////////////////////////////////////////////////////////////////////////

  template<p_policy A = default_policy, p_print P = default_print>
  class t_ctxt {
  public:
    t_ctxt();

    void set(t_id, p_what, const t_data2&);
    void set(t_id, p_what, const t_data1&);
    void set(const t_info&);

    t_id    get_id   () const;
    t_depth get_depth() const;
    p_what  get_what () const;
    t_info  get_info () const;

    t_info clear();

    void print(const t_data&) const;
    p_cstr what() const;

    void step_in (const t_data&, p_what);
    void step_out(const t_data&, p_what);
    void step_do (const t_data&, p_what);

  private:
    t_info info_;
  };

////////////////////////////////////////////////////////////////////////////////

  template<p_policy A, p_print P>
  inline
  t_ctxt<A, P>::t_ctxt() : info_(this) {
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::set(t_id id, p_what what, const t_data1& data) {
    A(info_.set(id, what, 0, data.tag_, 0, 0));
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::set(t_id id, p_what what, const t_data2& data) {
    A(info_.set(id, what, data.depth_, data.tag_, data.file_, data.line_));
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::set(const t_info& info) {
    info_ = info;
    A(info_);
  }

  template<p_policy A, p_print P>
  inline
  t_id t_ctxt<A, P>::get_id() const {
    return info_.id_;
  }

  template<p_policy A, p_print P>
  inline
  t_depth t_ctxt<A, P>::get_depth() const {
    return info_.depth_;
  }

  template<p_policy A, p_print P>
  inline
  p_what t_ctxt<A, P>::get_what() const {
    return info_.what_;
  }

  template<p_policy A, p_print P>
  inline
  t_info t_ctxt<A, P>::get_info() const {
    return info_;
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::print(const t_data& data) const {
    P(info_, data);
  }

  template<p_policy A, p_print P>
  inline
  p_cstr t_ctxt<A, P>::what() const {
    return info_.what_ ? "no oops" : info_.what_(info_.id_).string_;
  }

  template<p_policy A, p_print P>
  inline
  t_info t_ctxt<A, P>::clear() {
    t_info tmp = info_;
    info_.reset();
    return tmp;
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::step_in(const t_data& data, p_what what) {
    trace_step_in(info_, what, this, data);
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::step_out(const t_data& data, p_what what) {
    trace_step_out(info_, what, this, data);
  }

  template<p_policy A, p_print P>
  inline
  t_void t_ctxt<A, P>::step_do(const t_data& data, p_what what) {
    trace_step_do(info_, what, this, data);
  }
}
}

#endif
