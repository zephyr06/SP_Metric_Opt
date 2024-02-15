TEST(FiniteDist, AnalyzeFiniteDist_Float) {
    std::vector<float> data = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f, 9.9f, 10.0f};
    FiniteDist finite_dist = AnalyzeFiniteDist(data, 10);
    EXPECT_EQ(10, finite_dist.size());
    EXPECT_EQ(1.1f, finite_dist[0].value);
    EXPECT_EQ(0.1, finite_dist[0].probability);
    EXPECT_EQ(10.0f, finite_dist[9].value);
    EXPECT_EQ(1, finite_dist[9].probability);
}