//
// Created by darksider3 on 06.01.21.
//

#ifndef LIGI_APPS_LIPSUM_HPP
#define LIGI_APPS_LIPSUM_HPP
#include <iterator>
#include <ostream>
#include <string_view>

namespace {
constexpr std::string_view LipsumData = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec euismod mauris nec est lacinia blandit. Ut dui sapien, pulvinar sed vulputate quis, dignissim quis elit. Aliquam in varius quam. Donec ultricies nisl ligula, molestie mollis mi fringilla in. Nullam at augue enim. Sed eu dolor vestibulum lacus sodales imperdiet. Suspendisse rhoncus fermentum ligula, dictum faucibus lorem fermentum ac. Etiam et augue sed justo vehicula rutrum sed vitae purus. Mauris tempor iaculis nulla, sit amet hendrerit ipsum tristique id. In consectetur hendrerit malesuada.\n\n"
                                        "Quisque non tempor metus. Vivamus rhoncus lobortis vehicula. Integer feugiat dignissim diam et accumsan. Nulla fringilla tellus turpis, et tristique nibh sollicitudin sed. Suspendisse fringilla, quam pretium eleifend condimentum, turpis ante maximus leo, sit amet scelerisque diam felis id quam. Aliquam varius, leo et lobortis porttitor, est lorem interdum diam, non pellentesque nunc dui cursus eros. Duis ut mi at quam commodo interdum sit amet sit amet sem. Integer sit amet nibh eget purus cursus facilisis eu sodales lacus. Suspendisse eget urna id mi fringilla hendrerit sit amet sit amet sem. Aliquam venenatis cursus sapien non molestie. Curabitur et sagittis sapien. Praesent consequat massa in massa commodo rutrum. Phasellus viverra quis nisi at malesuada. Integer vestibulum odio ipsum. Duis et luctus justo. Aliquam felis ipsum, elementum vel ipsum efficitur, eleifend varius augue.\n\n"
                                        "Integer condimentum sit amet mauris nec vehicula. Curabitur posuere lacus sed venenatis rhoncus. Maecenas imperdiet blandit diam varius venenatis. In hac habitasse platea dictumst. Donec convallis nisi at dolor sagittis, sed malesuada quam accumsan. Integer ut orci finibus, commodo nunc sed, vehicula orci. Proin porta dolor vitae malesuada mollis. Nam massa tellus, sollicitudin nec erat suscipit, vestibulum placerat quam. Aenean in scelerisque magna. Sed hendrerit laoreet suscipit.\n\n"
                                        "Nullam id sodales nibh. Aenean sit amet congue sem, sed condimentum risus. Pellentesque sagittis sem sit amet ornare bibendum. Aliquam tincidunt id turpis sed venenatis. In in magna ultricies, sodales nisi in, tempor mi. Sed nulla urna, laoreet non pellentesque sit amet, porttitor sit amet massa. Pellentesque volutpat sem lorem, vitae fermentum ex pellentesque nec. Nulla porta velit nec ultricies porttitor. Proin imperdiet laoreet consectetur. Mauris id accumsan sem. Quisque risus est, faucibus nec lacus a, vulputate auctor est. Donec nec purus tincidunt, feugiat mauris id, feugiat metus. Cras laoreet nisi risus, at pulvinar sapien convallis id. Etiam nec mi et sem bibendum dictum sed rutrum ante. Curabitur placerat ornare nibh, ut ornare ligula vestibulum quis.\n\n"
                                        "Mauris commodo dui in est venenatis consequat. Etiam quis scelerisque orci, quis facilisis purus. Sed molestie quam ligula, vel commodo arcu rutrum ut. Sed efficitur at nibh venenatis euismod. Sed cursus, lorem non suscipit sagittis, sapien mauris cursus libero, sit amet luctus tellus nunc eget nunc. Nunc id dapibus urna, vitae placerat massa. Vestibulum lectus felis, pulvinar vel aliquet a, placerat pretium tellus. Nunc tempor, dolor non fringilla sagittis, purus nunc faucibus libero, at imperdiet augue elit nec erat. Vestibulum suscipit nulla metus, eu tincidunt metus scelerisque sit amet. Mauris sit amet leo vel velit condimentum convallis a sit amet eros. Etiam posuere odio enim, id malesuada sapien tincidunt ac. Nam velit velit, mattis sed ullamcorper tristique, accumsan vitae odio. Nam mi leo, sollicitudin eget eleifend ut, faucibus non nisl. Phasellus sed cursus leo. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Praesent nec mollis nibh.\n\n";

constexpr std::size_t Lipsum_Default_Len = 1024;

}
class lipsum;

class lipsum {
private:
    std::size_t m_default_gen_size; // BYTES!

public:
    lipsum(std::size_t default_len = Lipsum_Default_Len)
    {
        m_default_gen_size = default_len;
    }

    friend std::ostream& operator<<(std::ostream& in, lipsum& f)
    {
        std::string r;
        in << f.getBytes(f.getDefaultLen(), std::move(r));
        return in;
    }
    [[nodiscard]] std::size_t getDefaultLen() const { return m_default_gen_size; }
    [[nodiscard]] std::string&& getBytes(std::size_t size, std::string&& in) const;
};

#endif //LIGI_APPS_LIPSUM_HPP
