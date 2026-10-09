#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
// Spawned without SETPGROUP. This fresh executable, not a forked C++ thread,
// establishes the private terminal before replacing itself with the command.
int main(int argc, char **argv) {
  auto failure = [](int error) {
    const unsigned char tag = 2;
    (void)::write(3, &tag, sizeof(tag));
    (void)::write(3, &error, sizeof(error));
    _exit(127);
  };
  if (argc < 3)
    failure(EINVAL);
  if (::setsid() < 0)
    failure(errno);
  const int slave = ::open(argv[1], O_RDWR);
  if (slave < 0)
    failure(errno);
  if (::ioctl(slave, TIOCSCTTY, 0) < 0 || ::tcsetpgrp(slave, ::getpgrp()) < 0)
    failure(errno);
  for (int destination = 0; destination < 3; ++destination)
    if (::dup2(slave, destination) < 0)
      failure(errno);
  if (slave > 3)
    (void)::close(slave);
  if (::fcntl(3, F_SETFD, FD_CLOEXEC) < 0)
    failure(errno);
  const unsigned char ready = 1;
  if (::write(3, &ready, sizeof(ready)) != static_cast<ssize_t>(sizeof(ready)))
    failure(errno);
  // Reconstruct an owned argument vector at this executable boundary. No shell
  // interpretation occurs here; target and each argument remain distinct bytes.
  std::vector<std::string> arguments;
  std::vector<char *> target;
  try {
    for (int i = 2; i < argc; ++i)
      arguments.emplace_back(argv[i]);
    if (arguments.front().empty())
      failure(EINVAL);
    for (auto &argument : arguments)
      target.push_back(argument.data());
    target.push_back(nullptr);
  } catch (...) {
    failure(ENOMEM);
  }
  ::execvp(target.front(), target.data());
  failure(errno);
}
